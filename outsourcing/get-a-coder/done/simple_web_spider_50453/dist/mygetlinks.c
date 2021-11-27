/*
 * mygetlinks.c
 * all about fetching links
 */
#include "mygetlinks.h"


PRIVATE int printer (const char * fmt, va_list pArgs)
{
  return (vfprintf(stdout, fmt, pArgs));
}

PRIVATE int tracer (const char * fmt, va_list pArgs)
{
  return (vfprintf(stderr, fmt, pArgs));
}

PRIVATE int terminate_handler (HTRequest * request, HTResponse * response,
			       void * param, int status) 
{
  HTEventList_stopLoop();
  HTRequest_delete(request);
//  HTAnchor_delete(HTRequest_anchor(request));

  return HT_OK;
}

PRIVATE void foundLink (HText * text,
			int element_number, int attribute_number,
			HTChildAnchor * anchor,
			const BOOL * present, const char ** value)
{
  if (anchor) {
    /*
     **  Find out which link we got. The anchor we are passed is
     **  a child anchor of the anchor we are current parsing. We
     **  have to go from this child anchor to the actual destination.
     */
    HTAnchor * dest = HTAnchor_followMainLink((HTAnchor *) anchor);
    char * address = HTAnchor_address(dest);
    if (address) {
      do_something_with_link(address);
      //HTPrint("Found link `%s\'\n", address);
      HT_FREE(address);
    }
  }
}

/*  get links from the uri */
PUBLIC int getlinks(const char *uri)
{
  if (!uri) // what the hell is it!!!
    return 0;

  /* Handle command line args */
  HTRequest *request = NULL;
  HTAnchor *anchor = NULL;
  BOOL status = NO;

  /* Create a request */
  request = HTRequest_new();

  /* Get an anchor object for the URI */
  anchor = HTAnchor_findAddress(uri);

  /* Issue the GET and store the result in a chunk */
  status = HTLoadAnchor(anchor, request);

  /* Go into the event loop... */
  if (status == YES) {
    //HTEventList_loop(request);
    HTEventList_newLoop();
  }

  return 0; /* never return */
}

PUBLIC void init_libwww(const char *client, const char *version)
{
  /* Create a new premptive client */
  HTProfile_newHTMLNoCacheClient (client, version);

  /* Need our own trace and print functions */
  HTPrint_setCallback(printer);
  HTTrace_setCallback(tracer);

  /* Set trace messages and alert messages */
#if 0
  HTSetTraceMessageMask("sop");
#endif

  /* Add our own termination filter */
  HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

  /*
   ** Register our HTML link handler. We don't actually create a HText
   ** object as this is not needed. We only register the specific link
   ** callback.
   */
  HText_registerLinkCallback(foundLink);

  /* Setup a timeout on the request for 60 secs */
  HTHost_setEventTimeout(60000);

}

PUBLIC void destroy_libwww(void)
{
  /* Terminate libwww */
  HTProfile_delete();
}

