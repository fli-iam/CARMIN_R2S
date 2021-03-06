/* soapServer.cpp
   Generated by gSOAP 2.7.17 from api.h
   Copyright(C) 2000-2010, Robert van Engelen, Genivia Inc. All Rights Reserved.
   This part of the software is released under one of the following licenses:
   GPL, the gSOAP public license, or Genivia's license for commercial use.
*/

#if defined(__BORLANDC__)
#pragma option push -w-8060
#pragma option push -w-8004
#endif
#include "soapH.h"

SOAP_SOURCE_STAMP("@(#) soapServer.cpp ver 2.7.17 2016-03-11 10:04:49 GMT")


SOAP_FMAC5 int SOAP_FMAC6 soap_serve(struct soap *soap)
{
#ifndef WITH_FASTCGI
	unsigned int k = soap->max_keep_alive;
#endif

	do
	{
#ifdef WITH_FASTCGI
		if (FCGI_Accept() < 0)
		{
			soap->error = SOAP_EOF;
			return soap_send_fault(soap);
		}
#endif

		soap_begin(soap);

#ifndef WITH_FASTCGI
		if (soap->max_keep_alive > 0 && !--k)
			soap->keep_alive = 0;
#endif

		if (soap_begin_recv(soap))
		{	if (soap->error < SOAP_STOP)
			{
#ifdef WITH_FASTCGI
				soap_send_fault(soap);
#else 
				return soap_send_fault(soap);
#endif
			}
			soap_closesock(soap);

			continue;
		}

		if (soap_envelope_begin_in(soap)
		 || soap_recv_header(soap)
		 || soap_body_begin_in(soap)
		 || soap_serve_request(soap)
		 || (soap->fserveloop && soap->fserveloop(soap)))
		{
#ifdef WITH_FASTCGI
			soap_send_fault(soap);
#else
			return soap_send_fault(soap);
#endif
		}

#ifdef WITH_FASTCGI
		soap_destroy(soap);
		soap_end(soap);
	} while (1);
#else
	} while (soap->keep_alive);
#endif
	return SOAP_OK;
}

#ifndef WITH_NOSERVEREQUEST
SOAP_FMAC5 int SOAP_FMAC6 soap_serve_request(struct soap *soap)
{
	soap_peek_element(soap);
	if (!soap_match_tag(soap, soap->tag, "api:authenticateSession"))
		return soap_serve___api__authenticateSession(soap);
	if (!soap_match_tag(soap, soap->tag, "api:deleteExecution"))
		return soap_serve___api__deleteExecution(soap);
	if (!soap_match_tag(soap, soap->tag, "api:getPipeline"))
		return soap_serve___api__getPipeline(soap);
	if (!soap_match_tag(soap, soap->tag, "api:authenticateHTTP"))
		return soap_serve___api__authenticateHTTP(soap);
	if (!soap_match_tag(soap, soap->tag, "api:logout"))
		return soap_serve___api__logout(soap);
	if (!soap_match_tag(soap, soap->tag, "api:updateExecution"))
		return soap_serve___api__updateExecution(soap);
	if (!soap_match_tag(soap, soap->tag, "api:getGlobalProperties"))
		return soap_serve___api__getGlobalProperties(soap);
	if (!soap_match_tag(soap, soap->tag, "api:initExecution"))
		return soap_serve___api__initExecution(soap);
	if (!soap_match_tag(soap, soap->tag, "api:getStdOut"))
		return soap_serve___api__getStdOut(soap);
	if (!soap_match_tag(soap, soap->tag, "api:getStdErr"))
		return soap_serve___api__getStdErr(soap);
	if (!soap_match_tag(soap, soap->tag, "api:getExecutionResults"))
		return soap_serve___api__getExecutionResults(soap);
	if (!soap_match_tag(soap, soap->tag, "api:killExecution"))
		return soap_serve___api__killExecution(soap);
	if (!soap_match_tag(soap, soap->tag, "api:getExecution"))
		return soap_serve___api__getExecution(soap);
	if (!soap_match_tag(soap, soap->tag, "api:playExecution"))
		return soap_serve___api__playExecution(soap);
	if (!soap_match_tag(soap, soap->tag, "api:listPipelines"))
		return soap_serve___api__listPipelines(soap);
	if (!soap_match_tag(soap, soap->tag, "api:listExecutions"))
		return soap_serve___api__listExecutions(soap);
	return soap->error = SOAP_NO_METHOD;
}
#endif

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___api__authenticateSession(struct soap *soap)
{	struct __api__authenticateSession soap_tmp___api__authenticateSession;
	api__authenticateSessionResponse api__authenticateSessionResponse_;
	api__authenticateSessionResponse_.soap_default(soap);
	soap_default___api__authenticateSession(soap, &soap_tmp___api__authenticateSession);
	soap->encodingStyle = NULL;
	if (!soap_get___api__authenticateSession(soap, &soap_tmp___api__authenticateSession, "-api:authenticateSession", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __api__authenticateSession(soap, soap_tmp___api__authenticateSession.api__authenticateSession_, &api__authenticateSessionResponse_);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	api__authenticateSessionResponse_.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || api__authenticateSessionResponse_.soap_put(soap, "api:authenticateSessionResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || api__authenticateSessionResponse_.soap_put(soap, "api:authenticateSessionResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___api__deleteExecution(struct soap *soap)
{	struct __api__deleteExecution soap_tmp___api__deleteExecution;
	api__deleteExecutionResponse api__deleteExecutionResponse_;
	api__deleteExecutionResponse_.soap_default(soap);
	soap_default___api__deleteExecution(soap, &soap_tmp___api__deleteExecution);
	soap->encodingStyle = NULL;
	if (!soap_get___api__deleteExecution(soap, &soap_tmp___api__deleteExecution, "-api:deleteExecution", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __api__deleteExecution(soap, soap_tmp___api__deleteExecution.api__deleteExecution_, &api__deleteExecutionResponse_);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	api__deleteExecutionResponse_.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || api__deleteExecutionResponse_.soap_put(soap, "api:deleteExecutionResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || api__deleteExecutionResponse_.soap_put(soap, "api:deleteExecutionResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___api__getPipeline(struct soap *soap)
{	struct __api__getPipeline soap_tmp___api__getPipeline;
	api__getPipelineResponse api__getPipelineResponse_;
	api__getPipelineResponse_.soap_default(soap);
	soap_default___api__getPipeline(soap, &soap_tmp___api__getPipeline);
	soap->encodingStyle = NULL;
	if (!soap_get___api__getPipeline(soap, &soap_tmp___api__getPipeline, "-api:getPipeline", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __api__getPipeline(soap, soap_tmp___api__getPipeline.api__getPipeline_, &api__getPipelineResponse_);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	api__getPipelineResponse_.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || api__getPipelineResponse_.soap_put(soap, "api:getPipelineResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || api__getPipelineResponse_.soap_put(soap, "api:getPipelineResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___api__authenticateHTTP(struct soap *soap)
{	struct __api__authenticateHTTP soap_tmp___api__authenticateHTTP;
	api__authenticateHTTPResponse api__authenticateHTTPResponse_;
	api__authenticateHTTPResponse_.soap_default(soap);
	soap_default___api__authenticateHTTP(soap, &soap_tmp___api__authenticateHTTP);
	soap->encodingStyle = NULL;
	if (!soap_get___api__authenticateHTTP(soap, &soap_tmp___api__authenticateHTTP, "-api:authenticateHTTP", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __api__authenticateHTTP(soap, soap_tmp___api__authenticateHTTP.api__authenticateHTTP_, &api__authenticateHTTPResponse_);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	api__authenticateHTTPResponse_.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || api__authenticateHTTPResponse_.soap_put(soap, "api:authenticateHTTPResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || api__authenticateHTTPResponse_.soap_put(soap, "api:authenticateHTTPResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___api__logout(struct soap *soap)
{	struct __api__logout soap_tmp___api__logout;
	api__logoutResponse api__logoutResponse_;
	api__logoutResponse_.soap_default(soap);
	soap_default___api__logout(soap, &soap_tmp___api__logout);
	soap->encodingStyle = NULL;
	if (!soap_get___api__logout(soap, &soap_tmp___api__logout, "-api:logout", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __api__logout(soap, soap_tmp___api__logout.api__logout_, &api__logoutResponse_);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	api__logoutResponse_.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || api__logoutResponse_.soap_put(soap, "api:logoutResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || api__logoutResponse_.soap_put(soap, "api:logoutResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___api__updateExecution(struct soap *soap)
{	struct __api__updateExecution soap_tmp___api__updateExecution;
	api__updateExecutionResponse api__updateExecutionResponse_;
	api__updateExecutionResponse_.soap_default(soap);
	soap_default___api__updateExecution(soap, &soap_tmp___api__updateExecution);
	soap->encodingStyle = NULL;
	if (!soap_get___api__updateExecution(soap, &soap_tmp___api__updateExecution, "-api:updateExecution", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __api__updateExecution(soap, soap_tmp___api__updateExecution.api__updateExecution_, &api__updateExecutionResponse_);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	api__updateExecutionResponse_.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || api__updateExecutionResponse_.soap_put(soap, "api:updateExecutionResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || api__updateExecutionResponse_.soap_put(soap, "api:updateExecutionResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___api__getGlobalProperties(struct soap *soap)
{	struct __api__getGlobalProperties soap_tmp___api__getGlobalProperties;
	api__getGlobalPropertiesResponse api__getGlobalPropertiesResponse_;
	api__getGlobalPropertiesResponse_.soap_default(soap);
	soap_default___api__getGlobalProperties(soap, &soap_tmp___api__getGlobalProperties);
	soap->encodingStyle = NULL;
	if (!soap_get___api__getGlobalProperties(soap, &soap_tmp___api__getGlobalProperties, "-api:getGlobalProperties", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __api__getGlobalProperties(soap, soap_tmp___api__getGlobalProperties.api__getGlobalProperties_, &api__getGlobalPropertiesResponse_);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	api__getGlobalPropertiesResponse_.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || api__getGlobalPropertiesResponse_.soap_put(soap, "api:getGlobalPropertiesResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || api__getGlobalPropertiesResponse_.soap_put(soap, "api:getGlobalPropertiesResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___api__initExecution(struct soap *soap)
{	struct __api__initExecution soap_tmp___api__initExecution;
	api__initExecutionResponse api__initExecutionResponse_;
	api__initExecutionResponse_.soap_default(soap);
	soap_default___api__initExecution(soap, &soap_tmp___api__initExecution);
	soap->encodingStyle = NULL;
	if (!soap_get___api__initExecution(soap, &soap_tmp___api__initExecution, "-api:initExecution", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __api__initExecution(soap, soap_tmp___api__initExecution.api__initExecution_, &api__initExecutionResponse_);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	api__initExecutionResponse_.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || api__initExecutionResponse_.soap_put(soap, "api:initExecutionResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || api__initExecutionResponse_.soap_put(soap, "api:initExecutionResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___api__getStdOut(struct soap *soap)
{	struct __api__getStdOut soap_tmp___api__getStdOut;
	api__getStdOutResponse api__getStdOutResponse_;
	api__getStdOutResponse_.soap_default(soap);
	soap_default___api__getStdOut(soap, &soap_tmp___api__getStdOut);
	soap->encodingStyle = NULL;
	if (!soap_get___api__getStdOut(soap, &soap_tmp___api__getStdOut, "-api:getStdOut", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __api__getStdOut(soap, soap_tmp___api__getStdOut.api__getStdOut_, &api__getStdOutResponse_);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	api__getStdOutResponse_.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || api__getStdOutResponse_.soap_put(soap, "api:getStdOutResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || api__getStdOutResponse_.soap_put(soap, "api:getStdOutResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___api__getStdErr(struct soap *soap)
{	struct __api__getStdErr soap_tmp___api__getStdErr;
	api__getStdErrResponse api__getStdErrResponse_;
	api__getStdErrResponse_.soap_default(soap);
	soap_default___api__getStdErr(soap, &soap_tmp___api__getStdErr);
	soap->encodingStyle = NULL;
	if (!soap_get___api__getStdErr(soap, &soap_tmp___api__getStdErr, "-api:getStdErr", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __api__getStdErr(soap, soap_tmp___api__getStdErr.api__getStdErr_, &api__getStdErrResponse_);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	api__getStdErrResponse_.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || api__getStdErrResponse_.soap_put(soap, "api:getStdErrResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || api__getStdErrResponse_.soap_put(soap, "api:getStdErrResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___api__getExecutionResults(struct soap *soap)
{	struct __api__getExecutionResults soap_tmp___api__getExecutionResults;
	api__getExecutionResultsResponse api__getExecutionResultsResponse_;
	api__getExecutionResultsResponse_.soap_default(soap);
	soap_default___api__getExecutionResults(soap, &soap_tmp___api__getExecutionResults);
	soap->encodingStyle = NULL;
	if (!soap_get___api__getExecutionResults(soap, &soap_tmp___api__getExecutionResults, "-api:getExecutionResults", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __api__getExecutionResults(soap, soap_tmp___api__getExecutionResults.api__getExecutionResults_, &api__getExecutionResultsResponse_);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	api__getExecutionResultsResponse_.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || api__getExecutionResultsResponse_.soap_put(soap, "api:getExecutionResultsResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || api__getExecutionResultsResponse_.soap_put(soap, "api:getExecutionResultsResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___api__killExecution(struct soap *soap)
{	struct __api__killExecution soap_tmp___api__killExecution;
	api__killExecutionResponse api__killExecutionResponse_;
	api__killExecutionResponse_.soap_default(soap);
	soap_default___api__killExecution(soap, &soap_tmp___api__killExecution);
	soap->encodingStyle = NULL;
	if (!soap_get___api__killExecution(soap, &soap_tmp___api__killExecution, "-api:killExecution", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __api__killExecution(soap, soap_tmp___api__killExecution.api__killExecution_, &api__killExecutionResponse_);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	api__killExecutionResponse_.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || api__killExecutionResponse_.soap_put(soap, "api:killExecutionResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || api__killExecutionResponse_.soap_put(soap, "api:killExecutionResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___api__getExecution(struct soap *soap)
{	struct __api__getExecution soap_tmp___api__getExecution;
	api__getExecutionResponse api__getExecutionResponse_;
	api__getExecutionResponse_.soap_default(soap);
	soap_default___api__getExecution(soap, &soap_tmp___api__getExecution);
	soap->encodingStyle = NULL;
	if (!soap_get___api__getExecution(soap, &soap_tmp___api__getExecution, "-api:getExecution", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __api__getExecution(soap, soap_tmp___api__getExecution.api__getExecution_, &api__getExecutionResponse_);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	api__getExecutionResponse_.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || api__getExecutionResponse_.soap_put(soap, "api:getExecutionResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || api__getExecutionResponse_.soap_put(soap, "api:getExecutionResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___api__playExecution(struct soap *soap)
{	struct __api__playExecution soap_tmp___api__playExecution;
	api__playExecutionResponse api__playExecutionResponse_;
	api__playExecutionResponse_.soap_default(soap);
	soap_default___api__playExecution(soap, &soap_tmp___api__playExecution);
	soap->encodingStyle = NULL;
	if (!soap_get___api__playExecution(soap, &soap_tmp___api__playExecution, "-api:playExecution", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __api__playExecution(soap, soap_tmp___api__playExecution.api__playExecution_, &api__playExecutionResponse_);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	api__playExecutionResponse_.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || api__playExecutionResponse_.soap_put(soap, "api:playExecutionResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || api__playExecutionResponse_.soap_put(soap, "api:playExecutionResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___api__listPipelines(struct soap *soap)
{	struct __api__listPipelines soap_tmp___api__listPipelines;
	api__listPipelinesResponse api__listPipelinesResponse_;
	api__listPipelinesResponse_.soap_default(soap);
	soap_default___api__listPipelines(soap, &soap_tmp___api__listPipelines);
	soap->encodingStyle = NULL;
	if (!soap_get___api__listPipelines(soap, &soap_tmp___api__listPipelines, "-api:listPipelines", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __api__listPipelines(soap, soap_tmp___api__listPipelines.api__listPipelines_, &api__listPipelinesResponse_);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	api__listPipelinesResponse_.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || api__listPipelinesResponse_.soap_put(soap, "api:listPipelinesResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || api__listPipelinesResponse_.soap_put(soap, "api:listPipelinesResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve___api__listExecutions(struct soap *soap)
{	struct __api__listExecutions soap_tmp___api__listExecutions;
	api__listExecutionsResponse api__listExecutionsResponse_;
	api__listExecutionsResponse_.soap_default(soap);
	soap_default___api__listExecutions(soap, &soap_tmp___api__listExecutions);
	soap->encodingStyle = NULL;
	if (!soap_get___api__listExecutions(soap, &soap_tmp___api__listExecutions, "-api:listExecutions", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = __api__listExecutions(soap, soap_tmp___api__listExecutions.api__listExecutions_, &api__listExecutionsResponse_);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	api__listExecutionsResponse_.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || api__listExecutionsResponse_.soap_put(soap, "api:listExecutionsResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || api__listExecutionsResponse_.soap_put(soap, "api:listExecutionsResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

#if defined(__BORLANDC__)
#pragma option pop
#pragma option pop
#endif

/* End of soapServer.cpp */
