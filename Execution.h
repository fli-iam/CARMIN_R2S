#ifndef EXECUTION_H
#define EXECUTION_H

#include "CurlProcess.h"
#include <string>
#include <vector>

#include "soapH.h"

class Execution: public CurlProcess
{
    public:
      Execution();

      bool initExecution(
	struct soap *soap,
	ns1__initExecution* ns1__initExecution_);
      ns1__Execution * m_ns1__Execution;

      bool listExecutions(struct soap *soap,
	ns1__listExecutions* ns1__listExecutions_
      );
      std::vector<ns1__ArrayOfExecutions*> *m_returnedValueListExecutions;

      bool deleteExecution(struct soap *soap,
	ns1__deleteExecution*               ns1__deleteExecution_
      );
      
      bool playExecution(struct soap *soap,
	std::string executionId
      );
      

      // It will return to m_ns1__Execution
      bool getExecution(struct soap *soap,
	std::string executionId
      );

      ns1__ExecutionStatus convert_soma_workflow_status_to_carmin_status(std::string soma_workflow_status);
      
      // It will return to m_ns1__Execution
      bool killExecution(struct soap *soap,
	std::string executionId
      );

      bool getStdOutErr(struct soap *soap,
	std::string executionId
      );

      std::vector<std::string> * m_std_out;
      std::vector<std::string> * m_std_err;
};

#endif


