#ifndef EXECUTION_H
#define EXECUTION_H

#include "CurlProcess.h"
#include <string>
#include <vector>

#include "soapH.h"
#include "Config.h"

class Execution: public CurlProcess
{
    public:
      Execution(Config *pconfig);

      Config *m_pconfig;

      bool initExecution(
	struct soap *soap,
	api__initExecution* api__initExecution_);
      api__Execution * m_api__Execution;

      bool listExecutions(struct soap *soap,
	api__listExecutions* api__listExecutions_
      );
      std::vector<api__ArrayOfExecutions*> *m_returnedValueListExecutions;

      bool deleteExecution(struct soap *soap,
	api__deleteExecution*               api__deleteExecution_
      );

      bool playExecution(struct soap *soap,
	std::string executionId
      );

      // It will return to m_api__Execution
      bool getExecution(struct soap *soap,
	std::string executionId
      );

      api__ExecutionStatus convert_soma_workflow_status_to_carmin_status(std::string soma_workflow_status);

      // It will return to m_api__Execution
      bool killExecution(struct soap *soap,
	std::string executionId
      );

      bool getStdOutErr(struct soap *soap,
	std::string executionId
      );
 
      bool getStdOut(struct soap *soap,
        std::string executionId
      );

      bool getStdErr(
        struct soap *soap,
        std::string executionId
      );

      std::vector<std::string> * m_std_out;
      std::vector<std::string> * m_std_err;

      bool updateExecution(struct soap *soap,
	std::string executionId,
	std::string name,
	std::string value
      );
      
};

#endif


