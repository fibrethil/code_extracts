/**
 *	AutoStruct
 *
 *	Copyright (C) 2015  Christos Korras
 *
 *	This program is proprietary software: you cannot redistribute it and/or modify it.
 *
 *	This program is distributed WITHOUT ANY WARRANTY; without even the implied warranty of 
 *	FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	You should have received a copy of the Licenset along with this program.
 *
 *	If not, see <http://www.autostruct.com/licenses/>.
 *
**/

#include "distributive_server.h"

#include "distributive_autostructcalcsmanager.h"
#include <algs/include/local/distrmsgs_launchprocessevent.h>
#include <algs/include/local/distrmsgs_processoutputevent.h>
#include <algs/include/local/distrmsgs_processstartedevent.h>
#include <algs/include/local/distrmsgs_processfinishedevent.h>
#include <algs/include/local/distrmsgs_processprogressevent.h>
#include <algs/include/local/distrmsgs_processcancelevent.h>
#include <algs/include/local/distrmsgs_processcanceledevent.h>
#include <algs/include/local/distrmsgs_processerrorevent.h>
#include <threads/include/registry_mfdistralgsmailbox.hpp>
#include <threads/include/messages_closequeue.h>
#include <specs/include/applications_autostructcalcs.h>
#include <tools/include/logger_categories.h>

#include <QtGlobal>
#include <QtDebug>
#include <QJsonObject>
#include <QJsonDocument>

#include <iostream>
#include <memory>

	namespace /* unit */	{

	/*********************************************
	*
	* constants
	*
	**********************************************/
        char const* uc_pcAutoStructCalcsDir = "../../autostructcalcs";
        char const* uc_pcAutoStructCalcs = "autostructcalcs";
        char const* uc_pcAutoStructRc = "autostructrc";


	/*********************************************
	*
	* types
	*
	**********************************************/

	}	// namespace unit

	namespace algs		{
		namespace distributive		{

	/////////////helper classes//////////////////

	/*********************************************
	*
	* interface distributive_ServerImpl01
	*
	**********************************************/

	class distributive_Server::distributive_ServerImpl01		{
		// data
		private:
			distributive_Server* m_pServer;

            distributive_AutoStructCalcsManager* m_pAutoStructCalcsManager  { nullptr };

            std::shared_ptr<distrmsgs::distrmsgs_LaunchProcessEvent> m_spLaunchProcessEvent;
            std::shared_ptr<distrmsgs::distrmsgs_ProcessOutputEvent> m_spProcessOutputEvent;
            std::shared_ptr<distrmsgs::distrmsgs_ProcessCancelEvent> m_spProcessCancelEvent;

            threads::registry::registry_MFDistrAlgsMailBox::type m_MailBox;

            threads::registry::registry_MFCommandLineSender::type m_PyThreadAddress;
            threads::registry::registry_MFAlgsSender::type m_AlgsThreadAddress;
            threads::registry::registry_MFDbInstanceSender::type m_DbThreadAddress;
            threads::registry::registry_MFNetSender::type m_NetThreadAddress;
            threads::registry::registry_MFStdOutSender::type m_StdOutThreadAddress;
            threads::registry::registry_MFStdErrSender::type m_StdErrThreadAddress;

            void (distributive_ServerImpl01::*m_fState) ();

        // states
        private:
            void m_vProcessEventState();
            void m_vLaunchProcessState();
            void m_vProcessOutputState();
            void m_vProcessCancelState();

		// constructors
		public:
			distributive_ServerImpl01() = delete;
			distributive_ServerImpl01(distributive_Server*);
			distributive_ServerImpl01(distributive_ServerImpl01 const&) = delete;
			distributive_ServerImpl01(distributive_ServerImpl01&&) = delete;
			~distributive_ServerImpl01();

		// operators
		public:
			distributive_ServerImpl01& operator = (distributive_ServerImpl01 const&) = delete;
			distributive_ServerImpl01& operator = (distributive_ServerImpl01&&) = delete;
		
        // accessors
        public:
            threads::registry::registry_MFDistrAlgsSender::type m_GetMailBox();
		
        // operations
        public:
            void m_vRun();
            void m_vRegister(
                std::tuple<
                    threads::registry::registry_MFCommandLineSender::type,
                    threads::registry::registry_MFAlgsSender::type,
                    threads::registry::registry_MFDbInstanceSender::type,
                    threads::registry::registry_MFNetSender::type,
                    threads::registry::registry_MFStdOutSender::type,
                    threads::registry::registry_MFStdErrSender::type
                > const&
            );
	};

	/*********************************************
	*
	* implementation distributive_ServerImpl01	
	*
	**********************************************/
	// constructors
	distributive_Server::distributive_ServerImpl01::distributive_ServerImpl01(
        distributive_Server* ai_pServer
    )
		:	m_pServer(ai_pServer)
        ,   m_pAutoStructCalcsManager(new distributive_AutoStructCalcsManager(m_pServer))
	{
		Q_ASSERT(m_pServer);
	}

	distributive_Server::distributive_ServerImpl01::~distributive_ServerImpl01()    {
        std::cout
            <<  "I am Algs Distributive Server and I am dying!!!\n"
        ;
    }

    // operations
    void distributive_Server::distributive_ServerImpl01::m_vRun()   {
		Q_ASSERT(m_pServer);

        m_fState = &distributive_Server::distributive_ServerImpl01::m_vProcessEventState;
        try {
            for (;;)  {
                (this->*m_fState)();
            }
        }
        catch(threads::messages::messages_CloseQueue const&) {
//            Q_EMIT m_pServer->signal_vDistrAlgsThreadFinished();

            std::cout
                <<  "DistrAlgs thread is exiting!!!\n"
            ;

            m_pServer->deleteLater();
        }
    }

    void distributive_Server::distributive_ServerImpl01::m_vRegister(
        std::tuple<
            threads::registry::registry_MFCommandLineSender::type,
            threads::registry::registry_MFAlgsSender::type,
            threads::registry::registry_MFDbInstanceSender::type,
            threads::registry::registry_MFNetSender::type,
            threads::registry::registry_MFStdOutSender::type,
            threads::registry::registry_MFStdErrSender::type
        > const& airc_tupleThreadAddresses
    )   {
        auto [
            l_PyThreadAddress,
            l_AlgsThreadAddress,
            l_DbThreadAddress,
            l_NetThreadAddress,
            l_StdOutThreadAddress, 
            l_StdErrThreadAddress 
        ] = airc_tupleThreadAddresses;
        m_PyThreadAddress = l_PyThreadAddress; 
        m_AlgsThreadAddress = l_AlgsThreadAddress; 
        m_DbThreadAddress = l_DbThreadAddress; 
        m_NetThreadAddress = l_NetThreadAddress; 
        m_StdOutThreadAddress = l_StdOutThreadAddress; 
        m_StdErrThreadAddress = l_StdErrThreadAddress; 

        Q_ASSERT(!m_PyThreadAddress.isNull());
        Q_ASSERT(!m_AlgsThreadAddress.isNull());
        Q_ASSERT(!m_DbThreadAddress.isNull());
        Q_ASSERT(!m_NetThreadAddress.isNull());
        Q_ASSERT(!m_StdOutThreadAddress.isNull());
        Q_ASSERT(!m_StdErrThreadAddress.isNull());
    }

    // accessors
    threads::registry::registry_MFDistrAlgsSender::type
    distributive_Server::distributive_ServerImpl01::m_GetMailBox()    {
        return m_MailBox;
    }

    // states
    void distributive_Server::distributive_ServerImpl01::m_vProcessEventState()   {
        m_MailBox.wait()
            .handle<distrmsgs::distrmsgs_LaunchProcessEvent>(
                [this] (distrmsgs::distrmsgs_LaunchProcessEvent const& airc_LaunchProcessEvent) {
                    auto l_qsApplication =  airc_LaunchProcessEvent.mc_qsGetApplication();
                    Q_ASSERT(!l_qsApplication.isEmpty());

                    auto l_qslArguments = airc_LaunchProcessEvent.mc_qslGetArguments();

                    auto l_qjoProcessInput = airc_LaunchProcessEvent.mc_qjoGetProcessInput();

                    auto l_pqCommand = airc_LaunchProcessEvent.mc_pqGetCommand();
                    Q_ASSERT(l_pqCommand);

                    qDebug(logDebug).noquote()
                        <<  "the programs we are going to launch is: " + l_qsApplication
                        <<  "the arguments are: " + l_qslArguments.join("/")
                        <<  "the data input is: " + QString(QJsonDocument(l_qjoProcessInput).toJson())
                    ;

                    this->m_spLaunchProcessEvent = std::make_shared<
                            distrmsgs::distrmsgs_LaunchProcessEvent
                        >(
                            l_qsApplication,
                            l_qslArguments,
                            l_qjoProcessInput,
                            l_pqCommand
                        )
                    ;
                    m_fState = &distributive_Server::distributive_ServerImpl01::m_vLaunchProcessState;
                }
            )
            .handle<distrmsgs::distrmsgs_ProcessOutputEvent>(
                [this] (distrmsgs::distrmsgs_ProcessOutputEvent const& airc_ProcessOutputEvent) {
                    auto l_qsApplication =  airc_ProcessOutputEvent.mc_qsGetApplication();
                    Q_ASSERT(!l_qsApplication.isEmpty());

                    auto l_qjoOutput = airc_ProcessOutputEvent.mc_qjoGetOutput();

                    auto l_pqCommand = airc_ProcessOutputEvent.mc_pqGetCommand();
                    Q_ASSERT(l_pqCommand);

                    this->m_spProcessOutputEvent = std::make_shared<
                            distrmsgs::distrmsgs_ProcessOutputEvent
                        >(
                            l_qsApplication,
                            l_qjoOutput,
                            l_pqCommand
                        )
                    ;
                    m_fState = &distributive_Server::distributive_ServerImpl01::m_vProcessOutputState;
                }
            )
            .handle<distrmsgs::distrmsgs_ProcessErrorEvent>(
                [this] (distrmsgs::distrmsgs_ProcessErrorEvent const& airc_ProcessErrorEvent) {
                    Q_ASSERT(m_pServer);
                    Q_ASSERT(m_pAutoStructCalcsManager);

                    auto l_qsApplication =  airc_ProcessErrorEvent.mc_qsGetApplication();
                    Q_ASSERT(!l_qsApplication.isEmpty());

                    auto l_qjoError = airc_ProcessErrorEvent.mc_qjoGetError();

                    auto l_pqCommand = airc_ProcessErrorEvent.mc_pqGetCommand();
                    Q_ASSERT(l_pqCommand);

                    m_pAutoStructCalcsManager->m_vProcessError(l_qsApplication, l_qjoError, l_pqCommand);

                    Q_EMIT m_pServer->signal_vProcessError(l_qsApplication, l_qjoError, l_pqCommand);
                }
            )
            .handle<distrmsgs::distrmsgs_ProcessStartedEvent>(
                [this] (distrmsgs::distrmsgs_ProcessStartedEvent const& airc_ProcessStartedEvent) {
                    Q_ASSERT(m_pServer);

                    auto l_qsApplication =  airc_ProcessStartedEvent.mc_qsGetApplication();
                    Q_ASSERT(!l_qsApplication.isEmpty());

                    auto l_qjoStarted = airc_ProcessStartedEvent.mc_qjoGetStarted();

                    auto l_pqCommand = airc_ProcessStartedEvent.mc_pqGetCommand();
                    Q_ASSERT(l_pqCommand);

                    m_pAutoStructCalcsManager->m_vProcessStarted(l_qsApplication, l_qjoStarted);

                    Q_EMIT m_pServer->signal_vProcessStarted(l_qsApplication, l_pqCommand);
                }
            )
            .handle<distrmsgs::distrmsgs_ProcessFinishedEvent>(
                [this] (distrmsgs::distrmsgs_ProcessFinishedEvent const& airc_ProcessFinishedEvent) {
                    Q_ASSERT(m_pServer);
                    Q_ASSERT(m_pAutoStructCalcsManager);

                    auto l_qsApplication =  airc_ProcessFinishedEvent.mc_qsGetApplication();
                    Q_ASSERT(!l_qsApplication.isEmpty());

                    auto l_qjoFinished = airc_ProcessFinishedEvent.mc_qjoGetFinished();

                    auto l_pqCommand = airc_ProcessFinishedEvent.mc_pqGetCommand();
                    Q_ASSERT(l_pqCommand);

                    m_pAutoStructCalcsManager->m_vProcessFinished(l_qsApplication, l_qjoFinished, l_pqCommand);

                    Q_EMIT m_pServer->signal_vProcessFinished(l_qsApplication, l_qjoFinished, l_pqCommand);
                }
            )
            .handle<distrmsgs::distrmsgs_ProcessProgressEvent>(
                [this] (distrmsgs::distrmsgs_ProcessProgressEvent const& airc_ProcessProgressEvent) {
                    auto l_qsApplication =  airc_ProcessProgressEvent.mc_qsGetApplication();
                    Q_ASSERT(!l_qsApplication.isEmpty());

                    auto l_dProgress =  airc_ProcessProgressEvent.mc_dGetProgress();

                    auto l_pqCommand = airc_ProcessProgressEvent.mc_pqGetCommand();
                    Q_ASSERT(l_pqCommand);

                    Q_EMIT m_pServer->signal_vProcessProgress(l_qsApplication, l_dProgress, l_pqCommand);
                }
            )
            .handle<distrmsgs::distrmsgs_ProcessCancelEvent>(
                [this] (distrmsgs::distrmsgs_ProcessCancelEvent const& airc_ProcessCancelEvent) {
                    auto l_qsApplication =  airc_ProcessCancelEvent.mc_qsGetApplication();
                    Q_ASSERT(!l_qsApplication.isEmpty());

                    auto l_pqCommand = airc_ProcessCancelEvent.mc_pqGetCommand();
                    Q_ASSERT(l_pqCommand);

                    this->m_spProcessCancelEvent = std::make_shared<
                            distrmsgs::distrmsgs_ProcessCancelEvent
                        >(
                            l_qsApplication,
                            l_pqCommand
                        )
                    ;
                    m_fState = &distributive_Server::distributive_ServerImpl01::m_vProcessCancelState;
                }
            )
            .handle<distrmsgs::distrmsgs_ProcessCanceledEvent>(
                [this] (distrmsgs::distrmsgs_ProcessCanceledEvent const& airc_ProcessCanceledEvent) {
                    Q_ASSERT(m_pServer);
                    Q_ASSERT(m_pAutoStructCalcsManager);

                    auto l_qsApplication =  airc_ProcessCanceledEvent.mc_qsGetApplication();
                    Q_ASSERT(!l_qsApplication.isEmpty());

                    auto l_pqCommand = airc_ProcessCanceledEvent.mc_pqGetCommand();
                    Q_ASSERT(l_pqCommand);

                    m_pAutoStructCalcsManager->m_vProcessCanceled(l_qsApplication, QJsonObject(), l_pqCommand);

                    Q_EMIT m_pServer->signal_vProcessCanceled(l_qsApplication, l_pqCommand);
                }
            )
        ;
    }

    void distributive_Server::distributive_ServerImpl01::m_vLaunchProcessState()    {
        Q_ASSERT(m_spLaunchProcessEvent);
        Q_ASSERT(m_pAutoStructCalcsManager);

        auto l_qsApplication = m_spLaunchProcessEvent->mc_qsGetApplication();
        Q_ASSERT(!l_qsApplication.isEmpty());

        auto l_qslArguments = m_spLaunchProcessEvent->mc_qslGetArguments();

        auto l_qjoProcessInput = m_spLaunchProcessEvent->mc_qjoGetProcessInput();

        auto l_pqCommand = m_spLaunchProcessEvent->mc_pqGetCommand();
        Q_ASSERT(l_pqCommand);

        if (!m_pAutoStructCalcsManager->mc_bThreadAddressesRegistered()) {
            Q_ASSERT(!m_PyThreadAddress.isNull());
            Q_ASSERT(!m_AlgsThreadAddress.isNull());
            Q_ASSERT(!m_DbThreadAddress.isNull());
            Q_ASSERT(!m_NetThreadAddress.isNull());
            Q_ASSERT(!m_StdOutThreadAddress.isNull());
            Q_ASSERT(!m_StdErrThreadAddress.isNull());

            m_pAutoStructCalcsManager->m_vRegister(
                std::make_tuple(
                    m_PyThreadAddress,
                    m_AlgsThreadAddress,
                    m_pServer->m_GetMailBox(),
                    m_DbThreadAddress,
                    m_NetThreadAddress,
                    m_StdOutThreadAddress,
                    m_StdErrThreadAddress
                )
            );
        }
        
        qDebug(logDebug).noquote()
            <<  "the programs we are going to launch is: " + l_qsApplication
            <<  "the arguments are: " + l_qslArguments.join("/")
            <<  "the data input is: " + QString(QJsonDocument(l_qjoProcessInput).toJson())
        ;
        m_pAutoStructCalcsManager->m_vLaunchProcess(
            l_qsApplication,
            l_qslArguments,
            l_qjoProcessInput,
            l_pqCommand
        );

        m_spLaunchProcessEvent.reset();
        m_fState = &distributive_Server::distributive_ServerImpl01::m_vProcessEventState;
    }

    void distributive_Server::distributive_ServerImpl01::m_vProcessOutputState()    {
		Q_ASSERT(m_pServer);
        Q_ASSERT(m_spProcessOutputEvent);
        Q_ASSERT(m_pAutoStructCalcsManager);

        auto l_qsApplicationPath = m_spProcessOutputEvent->mc_qsGetApplication();
        Q_ASSERT(!l_qsApplicationPath.isEmpty());

        auto l_qjoOutput = m_spProcessOutputEvent->mc_qjoGetOutput();

        auto l_pqCommand = m_spProcessOutputEvent->mc_pqGetCommand();
        Q_ASSERT(l_pqCommand);

        m_pAutoStructCalcsManager->m_vProcessDone(l_qsApplicationPath, l_qjoOutput, l_pqCommand);

        auto l_qslApplicationPath = l_qsApplicationPath.split("/");
        Q_ASSERT(!l_qslApplicationPath.empty());

        auto l_qsApplication = l_qslApplicationPath.constLast();
        if (uc_pcAutoStructRc == l_qsApplication)   {
            Q_EMIT m_pServer->signal_vProcessDone(l_qsApplicationPath, l_qjoOutput, l_pqCommand);
        }

        m_spProcessOutputEvent.reset();
        m_fState = &distributive_Server::distributive_ServerImpl01::m_vProcessEventState;
    }

    void distributive_Server::distributive_ServerImpl01::m_vProcessCancelState()    {
        Q_ASSERT(m_spProcessCancelEvent);
        Q_ASSERT(m_pAutoStructCalcsManager);

        auto l_qsApplication = m_spProcessCancelEvent->mc_qsGetApplication();
        Q_ASSERT(!l_qsApplication.isEmpty());

        auto l_pqCommand = m_spProcessCancelEvent->mc_pqGetCommand();
        Q_ASSERT(l_pqCommand);

        if (!m_pAutoStructCalcsManager->mc_bThreadAddressesRegistered()) {
            Q_ASSERT(!m_PyThreadAddress.isNull());
            Q_ASSERT(!m_AlgsThreadAddress.isNull());
            Q_ASSERT(!m_DbThreadAddress.isNull());
            Q_ASSERT(!m_NetThreadAddress.isNull());
            Q_ASSERT(!m_StdOutThreadAddress.isNull());
            Q_ASSERT(!m_StdErrThreadAddress.isNull());

            m_pAutoStructCalcsManager->m_vRegister(
                std::make_tuple(
                    m_PyThreadAddress,
                    m_AlgsThreadAddress,
                    m_pServer->m_GetMailBox(),
                    m_DbThreadAddress,
                    m_NetThreadAddress,
                    m_StdOutThreadAddress,
                    m_StdErrThreadAddress
                )
            );
        }
        
        Q_EMIT m_pAutoStructCalcsManager->signal_vCancelProcess(l_qsApplication, l_pqCommand);

        m_spProcessCancelEvent.reset();
        m_fState = &distributive_Server::distributive_ServerImpl01::m_vProcessEventState;
    }

/////////////distributive_Server/////////////

// constructors
distributive_Server::distributive_Server(QObject* ai_pParent)
	:	QObject(ai_pParent)
{
	Q_ASSERT(!m_spServerImpl);
}

distributive_Server::~distributive_Server()	{}

// operations
void distributive_Server::m_vRegister(
    std::tuple<
        threads::registry::registry_MFCommandLineSender::type,
        threads::registry::registry_MFAlgsSender::type,
        threads::registry::registry_MFDbInstanceSender::type,
        threads::registry::registry_MFNetSender::type,
        threads::registry::registry_MFStdOutSender::type,
        threads::registry::registry_MFStdErrSender::type
    > const& airc_tupleThreadAddresses
)   {
	Q_ASSERT(m_spServerImpl);

    m_spServerImpl->m_vRegister(airc_tupleThreadAddresses);
}

// accessors

// delegates
void distributive_Server::slot_vOnStarted()    {
	Q_ASSERT(!m_spServerImpl);

    m_vInstantiatePimpl();
}

// helper methods
void distributive_Server::m_vInstantiatePimpl()  {
	Q_ASSERT(!m_spServerImpl);

	m_spServerImpl.reset(
		new distributive_ServerImpl01(this)
	);
	Q_ASSERT(m_spServerImpl);

   Q_EMIT signal_vMailBoxCreated(m_GetMailBox());

    m_spServerImpl->m_vRun();

}

threads::registry::registry_MFDistrAlgsSender::type
distributive_Server::m_GetMailBox()    {
	Q_ASSERT(m_spServerImpl);

    return m_spServerImpl->m_GetMailBox();
}


	}	// namespace distributive
}	// namespace algs
