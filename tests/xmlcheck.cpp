/*
 * xmlcheck.cpp
 *
 * $Id$
 *
 * Adapted from load-grammar-dom.cxx
 * Written by Boris Kolpackov <boris@codesynthesis.com>
 * Assigned, by the author, to the public domain
 *
 * This program uses Xerces-C++ DOM parser to load a set of schema files
 * and then to validate a set of XML documents against these schemas. To
 * build this program you will need Xerces-C++ 3.0.0 or later. For more
 * information, see:
 *
 * http: *www.codesynthesis.com/~boris/blog/2010/03/15/validating-external-schemas-xerces-cxx/
 *
 *
 * Adaptation by Keith Marshall <keithmarshall@users.sourceforge.net>
 * Copyright (C) 2013, MinGW.org Project
 *
 * This is free software.  Permission is granted to copy, modify and
 * redistribute this software, under the provisions of the GNU General
 * Public License, Version 3, (or, at your option, any later version),
 * as published by the Free Software Foundation; see the file COPYING
 * for licensing details.
 *
 * Note, in particular, that this software is provided "as is", in the
 * hope that it may prove useful, but WITHOUT WARRANTY OF ANY KIND; not
 * even an implied WARRANTY OF MERCHANTABILITY, nor of FITNESS FOR ANY
 * PARTICULAR PURPOSE.  Under no circumstances will the author, or the
 * MinGW Project, accept liability for any damages, however caused,
 * arising from the use of this software.
 *
 */
#include <cstdio>
#include <string>
#include <memory>       /* for std::auto_ptr */
#include <cstddef>      /* for std::size_t   */

#include <libgen.h>     /* for basename()    */

#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>

#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>

using namespace std;
using namespace xercesc;

#if _XERCES_VERSION < 30000
/* We need at least Xerces-C++ version 3.0.0
 */
# error Xerces-C++ version >= 3.0.0 is required!

#elif _XERCES_VERSION >= 30100
/* We may wish to exploit some features which were not introduced
 * until Xerces-C++ version 3.1.0
 */
# define IF_XERCES_30100_PLUS( STATEMENT )  STATEMENT

#else
/* We cannot use Xerces-C++ version 3.1.0 features; make them no-op.
 */
# define IF_XERCES_30100_PLUS( STATEMENT )
#endif

class error_handler: public DOMErrorHandler
{
  /* A locally defined class for capture of fault conditions, as
   * reported by our DOM parsers.
   */
  public:
    /* Constructor.
     */
    error_handler( const char *rel): source(rel),
      first_report(true), new_document(true), failed(false){}

    /* Method to access recorded error condition status.
     */
    bool has_failed() const { return failed; }

    /* Method to reset recorded status, in preparation for
     * parsing a new document.
     */
    void reset(){ new_document = true; failed = false; }

    /* Method to handle error conditions, on behalf of our
     * DOM parsers.
     */
    virtual bool handleError( const xercesc::DOMError& );

  private:
    /* The type of XML input being parsed, recorded when we
     * consturct the error handler for binding to a particular
     * DOM parser.
     */
    const char *source;

    /* Recording for error condition status.
     */
    bool first_report, new_document, failed;
};

bool
error_handler::handleError( const xercesc::DOMError& condition )
{
  /* Implementation of the error handler, which we will use to capture
   * status, and report abnormal conditions detected by our DOM parsers.
   */
  bool warn = condition.getSeverity() == DOMError::DOM_SEVERITY_WARNING;

  /* Record detection of any condition which is more severe than
   * a simple warning.
   */
  if( ! warn ) failed = true;

  /* Identify the location, within the current XML schema or document
   * file, where the abnormality has been detected.
   */
  DOMLocator* loc( condition.getLocation() );

  /* When this is the first abnormality detected within the current
   * XML schema or document file...
   */
  if( new_document )
  {
    /* ...but we've previously reported abnormalities within another
     * input file, then separate the current report from diagnostics
     * relating to that other file...
     */
    if( ! first_report ) fputc( '\n', stderr );

    /* ...then, regardless of whatever may have gone before, format
     * and emit a report header to identify the current file.
     */
    char *uri = XMLString::transcode( loc->getURI() );
    fprintf( stderr, "Problem Report:\n%s: %s\n", source, uri );
    XMLString::release( &uri );

    /* Record that we've now emitted a report header and diagnostic
     * for the current XML input file.
     */
    first_report = new_document = false;
  }

  /* Whether we added a new report header, or not, we still have a
   * diagnostic message to emit.
   */
  char* msg = XMLString::transcode( condition.getMessage() );
  fprintf( stderr, "%d:%d: %s: %s\n", loc->getLineNumber(),
      loc->getColumnNumber(), warn ? "WARNING" : "ERROR", msg
    );
  XMLString::release( &msg );

  /* Finally, we return "true" to tell the DOM parser that we've
   * handled the error, and that it should continue parsing.
   */
  return true;
}

static bool
insufficient_arguments( bool status, const char *program_pathname )
{
  /* Diagnostic routine to report a lack of any command arguments
   * to specify the XML documents which are to be validated.
   */
  if( status )
  {
    /* The "status" flag indicates an abnormal condition...
     *
     * We want to call "basename()" on the passed "program_pathname";
     * while this is likely safe, it MAY try to modify the input string,
     * so create a temporary working copy...
     */
    char progname[1 + strlen( program_pathname )];

    /* ...then format and emit an appropriate diagnostic message.
     */
    strcpy( progname, program_pathname );
    fprintf( stderr, "%s: no XML documents specified for validation\n"
	"usage: %s [schema.xsd ...] document.xml ...\n", basename( progname ),
	program_pathname
      );
  }
  /* Irrespective of condition, we echo back the input state.
   */
  return status;
}

DOMLSParser*
create_parser( XMLGrammarPool* pool )
{
  /* Helper function, to instantiate a DOM parser with "LS", (load and
   * save), capability, (although we intend to use only "load").
   */
  const XMLCh ls_id[] = { chLatin_L, chLatin_S, chNull };

  /* Locate a DOM implementation, providing the requisite "LS" feature.
   */
  DOMImplementation* impl(
    DOMImplementationRegistry::getDOMImplementation( ls_id ) );

  /* Instantiate a parser, based on this DOM implementation.
   */
  DOMLSParser* parser(
    impl->createLSParser(
      DOMImplementationLS::MODE_SYNCHRONOUS,
      0,
      XMLPlatformUtils::fgMemoryManager,
      pool ) );

  /* Retrieve a pointer to its configuration data...
   */
  DOMConfiguration* conf( parser->getDomConfig() );

  /* ...so we may apply this commonly useful configuration.
   */
  conf->setParameter( XMLUni::fgDOMComments, false );
  conf->setParameter( XMLUni::fgDOMDatatypeNormalization, true );
  conf->setParameter( XMLUni::fgDOMElementContentWhitespace, false );
  conf->setParameter( XMLUni::fgDOMNamespaces, true );
  conf->setParameter( XMLUni::fgDOMEntities, false );

  /* Enable validation.
   */
  conf->setParameter( XMLUni::fgDOMValidate, true );
  conf->setParameter( XMLUni::fgXercesSchema, true );
  conf->setParameter( XMLUni::fgXercesSchemaFullChecking, false );

  /* Use the loaded grammar during parsing.
   */
  conf->setParameter( XMLUni::fgXercesUseCachedGrammarInParse, true );

  /* Don't load schemas from any other source (e.g., from XML document's
   * xsi:schemaLocation attributes).
   */
  conf->setParameter( XMLUni::fgXercesLoadSchema, false );

  /* Xerces-C++ 3.1.0 is the first version with working support for
   * multiple import.
   */
  IF_XERCES_30100_PLUS(
      conf->setParameter( XMLUni::fgXercesHandleMultipleImports, true )
    );

  /* We will release the DOM document ourselves.
   */
  conf->setParameter( XMLUni::fgXercesUserAdoptsDOMDocument, true );

  /* Return a pointer to the instantiated parser.
   */
  return parser;
}

static inline int
validation_status( int argc, char **argv )
{
  int retcode = 0;

  /* Initialize a grammer pool, for use by our parser instances.
   */
  MemoryManager* mm( XMLPlatformUtils::fgMemoryManager );
  auto_ptr<XMLGrammarPool> gp( new XMLGrammarPoolImpl( mm ) );

  /* Load the schema definitions into the grammar pool.
   */
  int argind = 1;
  {
    /* Instantiate a parser for the schema definition file(s).
     */
    DOMLSParser* parser( create_parser( gp.get() ) );

    /* Initialize an error handler for the schema context,
     * and bind it to the schema file parser.
     */
    error_handler eh( "XML Schema" );
    parser->getDomConfig()->setParameter( XMLUni::fgDOMErrorHandler, &eh );

    /* Scan command arguments, left to right, to identify any XML schema
     * files which we are expected to interpret.
     */
    do { const char *source = argv[argind]; size_t extent = strlen( source );
	 if( (extent > 4) && (strcasecmp( source + extent - 4, ".xsd" ) == 0) )
	 {
	   /* We have a "*.xsd" file to parse; do so, loading the grammar...
	    */
	   if( !parser->loadGrammar( source, Grammar::SchemaGrammarType, true ) )
	   {
	     /* ...but complain, and bail out, if loading fails...
	      */
	     fprintf( stderr, "%s: error: unable to load\n", source );
	     retcode = 1;
	   }
	   if( eh.has_failed() )
	     /*
	      * ...or if any schema parsing error was encountered.
	      */
	     retcode = 1;
	 }
	 else
	   /* We've exhausted the "*.xsd" file references; break out of
	    * the scanning loop, without further ceremony.
	    */
	   break;

	 /* Continue for the next "*.xsd" file, if any, provided there
	  * have been no schema abormalities detected thus far.
	  */
       } while( (retcode == 0) && (++argind < argc) );

    /* We're finished with our schema parser; release its resource pool.
     */
    parser->release();
  }

  /* Before proceeding to parse any XML documents, check that any
   * specified XML schemas have been loaded successfully.
   */
  if( retcode == 0 )
  {
    /* It's okay to proceed, but it would be pointless to do so...
     */
    if( insufficient_arguments( argind >= argc, *argv ) )
      /*
       * ...when there are no remaining arguments to specify any
       * XML documents for checking; in this case, bail out.
       */
      return 1;

    /* Lock the grammar pool. This is necessary if we plan to use the
     * same grammar pool in multiple threads (this way we can reuse the
     * same grammar in multiple parsers). Locking the pool disallows any
     * modifications to the pool, such as an attempt by one of the threads
     * to cache additional schemas.
     */
    gp->lockPool();

    /* Instantiate a new parser, to process the XML documents.
     */
    DOMLSParser* parser( create_parser( gp.get() ) );

    /* Initialize an error handler for the XML document context,
     * and bind it to the new parser.
     */
    error_handler eh( "XML Document" );
    parser->getDomConfig()->setParameter( XMLUni::fgDOMErrorHandler, &eh );

    /* Process all remaining arguments, as references to XML documents.
     */
    while( argind < argc )
    {
      /* Reset the error handler state, prior to loading each document.
       */
      eh.reset();
      DOMDocument* doc( parser->parseURI( argv[argind++] ) );

      /* In this application, all we care about is that the document
       * can be successfully read by our validating parser; if we did
       * read it successfully, we have no further use for it, se we
       * may simply set it aside.
       */
      if( doc ) doc->release();

      /* If any error occurred, while parsing the current document,
       * the error handler will have recorded it; we need to capture
       * that state here, for our eventual return code.
       */
      if( eh.has_failed() ) retcode = 1;
    }
    /* When all specified documents have been validated, we are done
     * with our parser, so we may release its resource pool.
     */
    parser->release();
  }
  /* Report back, with the cumulative status from XML document parsing.
   */
  return retcode;
}

int
main( int argc, char **argv )
{
  /* Fewer than one argument, after the command verb itself,
   * is not useful; complain, and bail out.
   */
  if( insufficient_arguments( argc < 2, *argv ) )
    return 1;

  /* We must initialize Xerces-C++, before we can use it.
   */
  XMLPlatformUtils::Initialize();

  /* Determine the validation status for all specified XML documents,
   * with respect to any specified XML schema definitions.
   */
  int retcode = validation_status( argc, argv );

  /* Shut down the Xerces-C++ subsystem, before returning the resultant
   * validation status code to the operating system.
   */
  XMLPlatformUtils::Terminate();
  return retcode;
}

/* $RCSfile$: end of file */
