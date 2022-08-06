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

#include "autostructmanager_statemachine.h"

#include "autostructmanager_state.h"
#include <python/include/setup_main.h>
#include <algs/include/strategy_server.h>
#include <algs/include/distributive_server.h>
#include <db/include/vars_dbinstance.h>
#include <io/include/network_server.h>
#include <io/include/stdoutput_server.h>
#include <io/include/stderror_server.h>
#include <ui/include/windows_mainwindow.h>
#include <threads/include/messages_closequeue.h>
#include <events/include/filesceneevents_toexitautostructevent.h>
#include <tools/include/logger_categories.h>

#include <QtGlobal>
#include <QtDebug>
#include <QState>
#include <QFinalState>
#include <QThread>
#include <QJsonObject>
#include <QMetaType>
#include <QStringList>
#include <QCoreApplication>

#include <thread>
#include <iostream>
#include <memory>

namespace uiw = ui::windows;


	namespace /* unit */	{

	/*********************************************
	*
	* constants
	*
	**********************************************/


	/*********************************************
	*
	* types
	*
	**********************************************/

	}	// namespace unit

	namespace states		{
		namespace autostructmanager		{

	/////////////helper classes//////////////////

	/*********************************************
	*
	* interface autostructmanager_StateMachineImpl01
	*
	**********************************************/

	class autostructmanager_StateMachine::autostructmanager_StateMachineImpl01    {
		// data
		private:
            // interface object
            autostructmanager_StateMachine* m_pManagingStateMachine;

            QThread* m_pqPyThread   {nullptr};
            QThread* m_pqAlgsServerThread   {nullptr};
            QThread* m_pqDistrAlgsServerThread   {nullptr};
            QThread* m_pqDbThread   {nullptr};
            QThread* m_pqNetServerThread   {nullptr};
            QThread* m_pqStdOutServerThread   {nullptr};
            QThread* m_pqStdErrServerThread   {nullptr};

            python::setup::setup_Main* m_pPythonMain    {nullptr};
            algs::strategy::strategy_Server* m_pAlgsServer   {nullptr};
            algs::distributive::distributive_Server* m_pDistrAlgsServer   {nullptr};
            db::vars::vars_DbInstance* m_pDbInstance    {nullptr};
            io::network::network_Server* m_pNetServer   {nullptr};
            io::stdoutput::stdoutput_Server* m_pStdOutServer   {nullptr};
            io::stderror::stderror_Server* m_pStdErrServer   {nullptr};

            threads::registry::registry_MFCommandLineSender::type m_PyThreadAddress;
            threads::registry::registry_MFAlgsSender::type m_AlgsThreadAddress;
            threads::registry::registry_MFDistrAlgsSender::type m_DistrAlgsThreadAddress;
            threads::registry::registry_MFDbInstanceSender::type m_DbThreadAddress;
            threads::registry::registry_MFNetSender::type m_NetThreadAddress;
            threads::registry::registry_MFStdOutSender::type m_StdOutThreadAddress;
            threads::registry::registry_MFStdErrSender::type m_StdErrThreadAddress;

            std::unique_ptr<uiw::windows_MainWindow> m_upMainWindow;

            int m_iMailBoxesCreatedNumber   {0};
            int m_iRunningThreads   {0};

        // helper methods
        private:
            void m_vRegisterMetaTypes();
            void m_vDisplayPyThreadDlg(QJsonObject const& airc_qjoPyThreadError);
            void m_vDisplayAlgsThreadDlg(QJsonObject const& airc_qjoAlgsThreadError);
            void m_vDisplayDistrAlgsThreadDlg(QJsonObject const& airc_qjoDistrAlgsThreadError);
            void m_vDisplayDbInstanceThreadDlg(QJsonObject const& airc_qjoDbInstanceThreadError);
            void m_vDisplayNetThreadDlg(QJsonObject const& airc_qjoNetThreadError);
            void m_vDisplayStdOutThreadDlg(QJsonObject const& airc_qjoStdOutThreadError);
            void m_vDisplayStdErrThreadDlg(QJsonObject const& airc_qjoStdErrThreadError);

            void m_vSendErrorToStdErrThread(QJsonObject const& airc_qjoPyThreadError);
            void m_vSendErrorToWebServer(QJsonObject const& airc_qjoPyThreadError);

            void m_vCreateStates();
            void m_vRegisterMainWindow();

            void m_vGetAddresses();
            void m_vRegisterAddresses();
            void m_vMoveWorkersToThreads();
            void m_vLaunchThreads();
            bool mc_bIsRunning() const;

            void m_vPlugInSignalsSlotsPyThread();
            void m_vPlugInSignalsSlotsAlgsThread();
            void m_vPlugInSignalsSlotsDistrAlgsThread();
            void m_vPlugInSignalsSlotsDbInstancesThread();
            void m_vPlugInSignalsSlotsNetThread();
            void m_vPlugInSignalsSlotsStdOutThread();
            void m_vPlugInSignalsSlotsStdErrThread();

		// constructors
		public:
			autostructmanager_StateMachineImpl01() = delete;
			autostructmanager_StateMachineImpl01(autostructmanager_StateMachine*);
			autostructmanager_StateMachineImpl01(autostructmanager_StateMachineImpl01 const&) = delete;
			autostructmanager_StateMachineImpl01(autostructmanager_StateMachineImpl01&&) = delete;
			~autostructmanager_StateMachineImpl01();

		// operators
		public:
			autostructmanager_StateMachineImpl01& operator = (autostructmanager_StateMachineImpl01 const&) = delete;
			autostructmanager_StateMachineImpl01& operator = (autostructmanager_StateMachineImpl01&&) = delete;

        // operations
        public:
            void m_vStartStateMachine();
            void m_vOnEntry(QEvent* ai_pEvent);
            void m_vOnExit(QEvent* ai_pEvent);

            void m_vThreadStarted();
            void m_vThreadFinished();

            void m_vPyThreadFinished();
            void m_vPyThreadError(QJsonObject const& airc_qjoPyThreadError);

            void m_vAlgsThreadFinished();
            void m_vAlgsThreadError(QJsonObject const& airc_qjoAlgsThreadError);

            void m_vDistrAlgsThreadFinished();
            void m_vDistrAlgsThreadError(QJsonObject const& airc_qjoDistrAlgsThreadError);

            void m_vDbInstanceThreadFinished();
            void m_vDbInstanceThreadError(QJsonObject const& airc_qjoDbInstanceThreadError);

            void m_vNetThreadFinished();
            void m_vNetThreadError(QJsonObject const& airc_qjoNetThreadError);

            void m_vStdOutThreadFinished();
            void m_vStdOutThreadError(QJsonObject const& airc_qjoStdOutThreadError);

            void m_vStdErrThreadFinished();
            void m_vStdErrThreadError(QJsonObject const& airc_qjoStdErrThreadError);

            void m_vPyThreadMailBoxCreated(threads::registry::registry_MFCommandLineSender::type);
            void m_vAlgsThreadMailBoxCreated(threads::registry::registry_MFAlgsSender::type);
            void m_vDistrAlgsThreadMailBoxCreated(threads::registry::registry_MFDistrAlgsSender::type);
            void m_vDbThreadMailBoxCreated(threads::registry::registry_MFDbInstanceSender::type);
            void m_vNetThreadMailBoxCreated(threads::registry::registry_MFNetSender::type m_NetThreadAddress);
            void m_vStdOutThreadMailBoxCreated(threads::registry::registry_MFStdOutSender::type);
            void m_vStdErrThreadMailBoxCreated(threads::registry::registry_MFStdErrSender::type);

        // delegates
        public:
            void m_vOnCloseThreads();

	};

	/*********************************************
	*
	* implementation autostructmanager_StateMachineImpl01	
	*
	**********************************************/
	// constructors
	autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::autostructmanager_StateMachineImpl01(
        autostructmanager_StateMachine* ai_pStateMachine
    )
		:	m_pManagingStateMachine(ai_pStateMachine)
        ,   m_pqPyThread(new QThread(nullptr))
        ,   m_pqAlgsServerThread(new QThread(nullptr))
        ,   m_pqDistrAlgsServerThread(new QThread(nullptr))
        ,   m_pqDbThread(new QThread(nullptr))
        ,   m_pqNetServerThread(new QThread(nullptr))
        ,   m_pqStdOutServerThread(new QThread(nullptr))
        ,   m_pqStdErrServerThread(new QThread(nullptr))
        ,   m_pPythonMain(new python::setup::setup_Main(nullptr))
        ,   m_pAlgsServer(new algs::strategy::strategy_Server(nullptr))
        ,   m_pDistrAlgsServer(new algs::distributive::distributive_Server(nullptr))
        ,   m_pDbInstance(new db::vars::vars_DbInstance(nullptr))
        ,   m_pNetServer(new io::network::network_Server(nullptr))
        ,   m_pStdOutServer(new io::stdoutput::stdoutput_Server(nullptr))
        ,   m_pStdErrServer(new io::stderror::stderror_Server(nullptr))
        ,   m_upMainWindow(
                new uiw::windows_MainWindow(
                    m_pManagingStateMachine
                )
            )
	{
        Q_ASSERT(m_pManagingStateMachine);
        Q_ASSERT(m_upMainWindow);

        m_upMainWindow->m_vEnableActions();

        m_vRegisterMetaTypes();

        m_vMoveWorkersToThreads();
        m_vPlugInSignalsSlotsPyThread();
        m_vPlugInSignalsSlotsAlgsThread();
        m_vPlugInSignalsSlotsDistrAlgsThread();
        m_vPlugInSignalsSlotsDbInstancesThread();
        m_vPlugInSignalsSlotsNetThread();
        m_vPlugInSignalsSlotsStdOutThread();
        m_vPlugInSignalsSlotsStdErrThread();

        m_vGetAddresses();

        m_vLaunchThreads();

        std::cout
            <<  ("the number of threads supported by this machine is: " + QString::number(std::thread::hardware_concurrency())).toStdString().c_str()
            <<  std::endl
        ;

        m_upMainWindow->show();

	}

	autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::
    ~autostructmanager_StateMachineImpl01()    {
        qDebug(logDebug).noquote()
            <<  "state machine destructor here!!!"
        ;
    }

    // delegates
    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vOnCloseThreads()    {
        Q_ASSERT(m_pqPyThread);
        Q_ASSERT(m_pqAlgsServerThread);
        Q_ASSERT(m_pqDistrAlgsServerThread);
        Q_ASSERT(m_pqDbThread);
        Q_ASSERT(m_pqNetServerThread);
        Q_ASSERT(m_pqStdOutServerThread);
        Q_ASSERT(m_pqStdErrServerThread);
        Q_ASSERT(!m_PyThreadAddress.isNull());
        Q_ASSERT(!m_AlgsThreadAddress.isNull());
        Q_ASSERT(!m_DistrAlgsThreadAddress.isNull());
        Q_ASSERT(!m_DbThreadAddress.isNull());
        Q_ASSERT(!m_NetThreadAddress.isNull());
        Q_ASSERT(!m_StdOutThreadAddress.isNull());
        Q_ASSERT(!m_StdErrThreadAddress.isNull());

        m_PyThreadAddress.send(threads::messages::messages_CloseQueue());
        m_AlgsThreadAddress.send(threads::messages::messages_CloseQueue());
        m_DistrAlgsThreadAddress.send(threads::messages::messages_CloseQueue());
        m_DbThreadAddress.send(threads::messages::messages_CloseQueue());
        m_NetThreadAddress.send(threads::messages::messages_CloseQueue());
        m_StdOutThreadAddress.send(threads::messages::messages_CloseQueue());
        m_StdErrThreadAddress.send(threads::messages::messages_CloseQueue());

        m_vPyThreadFinished();
        m_vAlgsThreadFinished();
        m_vDistrAlgsThreadFinished();
        m_vDbInstanceThreadFinished();
        m_vNetThreadFinished();
        m_vStdOutThreadFinished();
        m_vStdErrThreadFinished();
    }

    // opeartions
    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vStartStateMachine() {
		Q_ASSERT(m_pManagingStateMachine);

        m_pManagingStateMachine->start();

        mc_bIsRunning();
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vThreadStarted()    {
        ++m_iRunningThreads;
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vThreadFinished()    {
		Q_ASSERT(m_pManagingStateMachine);

        --m_iRunningThreads;

        if (0 == m_iRunningThreads) {
            qDebug(logDebug).noquote()
                <<  "m_iRunningThreads = 0...time to quit"
            ;
            QCoreApplication::quit();
        }
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vPyThreadFinished()    {
        Q_ASSERT(m_pqPyThread);
        Q_ASSERT(m_pPythonMain);

        qDebug(logDebug).noquote()
            <<  "python thread is about to quit the loop!!!"
        ;
        m_pqPyThread->quit();
        m_pqPyThread->wait();

        m_pqPyThread->deleteLater();

        qDebug(logDebug).noquote()
            <<  "python thread is scheduled for elimination!!!"
        ;
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vPyThreadError(
        QJsonObject const& airc_qjoPyThreadError
    )    {
        Q_ASSERT(m_upMainWindow);

        m_vDisplayPyThreadDlg(airc_qjoPyThreadError);
        m_vSendErrorToStdErrThread(airc_qjoPyThreadError);
        m_vSendErrorToWebServer(airc_qjoPyThreadError);
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vAlgsThreadFinished()    {
        Q_ASSERT(m_pqAlgsServerThread);
        Q_ASSERT(m_pAlgsServer);

        qDebug(logDebug).noquote()
            <<  "algs thread is about to quit the loop!!!"
        ;
        m_pqAlgsServerThread->quit();
        m_pqAlgsServerThread->wait();

        m_pqAlgsServerThread->deleteLater();

        qDebug(logDebug).noquote()
            <<  "algs strategy thread is scheduled for elimination!!!"
        ;
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vAlgsThreadError(
        QJsonObject const& airc_qjoAlgsThreadError
    )    {
        Q_ASSERT(m_upMainWindow);

        m_vDisplayAlgsThreadDlg(airc_qjoAlgsThreadError);
        m_vSendErrorToStdErrThread(airc_qjoAlgsThreadError);
        m_vSendErrorToWebServer(airc_qjoAlgsThreadError);
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vDistrAlgsThreadFinished()    {
        Q_ASSERT(m_pqDistrAlgsServerThread);
        Q_ASSERT(m_pDistrAlgsServer);

        qDebug(logDebug).noquote()
            <<  "distr algs thread is about to quit the loop!!!"
        ;
        m_pqDistrAlgsServerThread->quit();
        m_pqDistrAlgsServerThread->wait();

        m_pqDistrAlgsServerThread->deleteLater();

        qDebug(logDebug).noquote()
            <<  "distributive algs thread is scheduled for elimination!!!"
        ;

    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vDistrAlgsThreadError(
        QJsonObject const& airc_qjoDistrAlgsThreadError
    )    {
        Q_ASSERT(m_upMainWindow);

        m_vDisplayDistrAlgsThreadDlg(airc_qjoDistrAlgsThreadError);
        m_vSendErrorToStdErrThread(airc_qjoDistrAlgsThreadError);
        m_vSendErrorToWebServer(airc_qjoDistrAlgsThreadError);
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vDbInstanceThreadFinished()    {
        Q_ASSERT(m_pqDbThread);
        Q_ASSERT(m_pDbInstance);

        qDebug(logDebug).noquote()
            <<  "db thread is about to quit the loop!!!"
        ;
        m_pqDbThread->quit();
        m_pqDbThread->wait();

        m_pqDbThread->deleteLater();

        qDebug(logDebug).noquote()
            <<  "db thread is scheduled for elimination!!!"
        ;
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vDbInstanceThreadError(
        QJsonObject const& airc_qjoDbInstanceThreadError
    )    {
        Q_ASSERT(m_upMainWindow);

        m_vDisplayDbInstanceThreadDlg(airc_qjoDbInstanceThreadError);
        m_vSendErrorToStdErrThread(airc_qjoDbInstanceThreadError);
        m_vSendErrorToWebServer(airc_qjoDbInstanceThreadError);
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vNetThreadFinished()    {
        Q_ASSERT(m_pqNetServerThread);
        Q_ASSERT(m_pNetServer);

        qDebug(logDebug).noquote()
            <<  "net thread is about to quit the loop!!!"
        ;

        m_pqNetServerThread->quit();
        m_pqNetServerThread->wait();

        m_pqNetServerThread->deleteLater();

        qDebug(logDebug).noquote()
            <<  "network thread is scheduled for elimination!!!"
        ;
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vNetThreadError(
        QJsonObject const& airc_qjoNetThreadError
    )    {
        Q_ASSERT(m_upMainWindow);

        m_vDisplayNetThreadDlg(airc_qjoNetThreadError);
        m_vSendErrorToStdErrThread(airc_qjoNetThreadError);
        m_vSendErrorToWebServer(airc_qjoNetThreadError);
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vStdOutThreadFinished()    {
        Q_ASSERT(m_pqStdOutServerThread);
        Q_ASSERT(m_pStdOutServer);

        qDebug(logDebug).noquote()
            <<  "output log thread is about to quit the loop!!!"
        ;
        m_pqStdOutServerThread->quit();
        m_pqStdOutServerThread->wait();

        m_pqStdOutServerThread->deleteLater();

        qDebug(logDebug).noquote()
            <<  "output log  thread is scheduled for elimination!!!"
        ;
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vStdOutThreadError(
        QJsonObject const& airc_qjoStdOutThreadError
    )    {
        Q_ASSERT(m_upMainWindow);

        m_vDisplayStdOutThreadDlg(airc_qjoStdOutThreadError);
        m_vSendErrorToStdErrThread(airc_qjoStdOutThreadError);
        m_vSendErrorToWebServer(airc_qjoStdOutThreadError);
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vStdErrThreadFinished()    {
        Q_ASSERT(m_pqStdErrServerThread);
        Q_ASSERT(m_pStdErrServer);

        qDebug(logDebug).noquote()
            <<  "error log thread is about to quit the loop!!!"
        ;
        m_pqStdErrServerThread->quit();
        m_pqStdErrServerThread->wait();

        m_pqStdErrServerThread->deleteLater();

        qDebug(logDebug).noquote()
            <<  "error log  thread is scheduled for elimination!!!"
        ;
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vStdErrThreadError(
        QJsonObject const& airc_qjoStdErrThreadError
    )    {
        Q_ASSERT(m_upMainWindow);

        m_vDisplayStdErrThreadDlg(airc_qjoStdErrThreadError);
        m_vSendErrorToStdErrThread(airc_qjoStdErrThreadError);
        m_vSendErrorToWebServer(airc_qjoStdErrThreadError);
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vPyThreadMailBoxCreated(
        threads::registry::registry_MFCommandLineSender::type ai_cMailBox
    )   {
        Q_ASSERT(m_PyThreadAddress.isNull());
        Q_ASSERT(7 > m_iMailBoxesCreatedNumber);

        m_PyThreadAddress = ai_cMailBox;

        Q_ASSERT(!m_PyThreadAddress.isNull());

        ++m_iMailBoxesCreatedNumber;
        if (7 == m_iMailBoxesCreatedNumber) {
            m_vRegisterAddresses();
            m_vCreateStates();
            m_vRegisterMainWindow();
        }
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vAlgsThreadMailBoxCreated(
        threads::registry::registry_MFAlgsSender::type ai_cMailBox
    )   {
        Q_ASSERT(m_AlgsThreadAddress.isNull());
        Q_ASSERT(7 > m_iMailBoxesCreatedNumber);

        m_AlgsThreadAddress = ai_cMailBox;

        Q_ASSERT(!m_AlgsThreadAddress.isNull());

        ++m_iMailBoxesCreatedNumber;
        if (7 == m_iMailBoxesCreatedNumber) {
            m_vRegisterAddresses();
            m_vCreateStates();
            m_vRegisterMainWindow();
        }
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::
    m_vDistrAlgsThreadMailBoxCreated(threads::registry::registry_MFDistrAlgsSender::type ai_cMailBox)   {
        Q_ASSERT(m_DistrAlgsThreadAddress.isNull());
        Q_ASSERT(7 > m_iMailBoxesCreatedNumber);

        m_DistrAlgsThreadAddress = ai_cMailBox;

        Q_ASSERT(!m_DistrAlgsThreadAddress.isNull());

        ++m_iMailBoxesCreatedNumber;
        if (7 == m_iMailBoxesCreatedNumber) {
            m_vRegisterAddresses();
            m_vCreateStates();
            m_vRegisterMainWindow();
        }
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::
    m_vDbThreadMailBoxCreated(threads::registry::registry_MFDbInstanceSender::type ai_cMailBox) {
        Q_ASSERT(m_DbThreadAddress.isNull());
        Q_ASSERT(7 > m_iMailBoxesCreatedNumber);

        m_DbThreadAddress = ai_cMailBox;

        Q_ASSERT(!m_DbThreadAddress.isNull());

        ++m_iMailBoxesCreatedNumber;
        if (7 == m_iMailBoxesCreatedNumber) {
            m_vRegisterAddresses();
            m_vCreateStates();
            m_vRegisterMainWindow();
        }
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::
    m_vNetThreadMailBoxCreated(
        threads::registry::registry_MFNetSender::type ai_cMailBox
    )    {
        Q_ASSERT(m_NetThreadAddress.isNull());
        Q_ASSERT(7 > m_iMailBoxesCreatedNumber);

        m_NetThreadAddress = ai_cMailBox;

        Q_ASSERT(!m_NetThreadAddress.isNull());

        ++m_iMailBoxesCreatedNumber;
        if (7 == m_iMailBoxesCreatedNumber) {
            m_vRegisterAddresses();
            m_vCreateStates();
            m_vRegisterMainWindow();
        }
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::
    m_vStdOutThreadMailBoxCreated(threads::registry::registry_MFStdOutSender::type ai_cMailBox) {
        Q_ASSERT(m_StdOutThreadAddress.isNull());
        Q_ASSERT(7 > m_iMailBoxesCreatedNumber);

        m_StdOutThreadAddress = ai_cMailBox;

        Q_ASSERT(!m_StdOutThreadAddress.isNull());

        ++m_iMailBoxesCreatedNumber;
        if (7 == m_iMailBoxesCreatedNumber) {
            m_vRegisterAddresses();
            m_vCreateStates();
            m_vRegisterMainWindow();
        }
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::
    m_vStdErrThreadMailBoxCreated(threads::registry::registry_MFStdErrSender::type ai_cMailBox) {
        Q_ASSERT(m_StdErrThreadAddress.isNull());
        Q_ASSERT(7 > m_iMailBoxesCreatedNumber);

        m_StdErrThreadAddress = ai_cMailBox;

        Q_ASSERT(!m_StdErrThreadAddress.isNull());

        ++m_iMailBoxesCreatedNumber;
        if (7 == m_iMailBoxesCreatedNumber) {
            m_vRegisterAddresses();
            m_vCreateStates();
            m_vRegisterMainWindow();
        }
    }



    // overriding functionalities
    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vOnEntry(QEvent* ai_pEvent)  {
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vOnExit(QEvent* ai_pEvent)   {
    }

    // helper methods
    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vCreateStates()  {
		Q_ASSERT(m_pManagingStateMachine);
        Q_ASSERT(m_upMainWindow);

        Q_ASSERT(!m_PyThreadAddress.isNull());
        Q_ASSERT(!m_AlgsThreadAddress.isNull());
        Q_ASSERT(!m_DistrAlgsThreadAddress.isNull());
        Q_ASSERT(!m_DbThreadAddress.isNull());
        Q_ASSERT(!m_NetThreadAddress.isNull());
        Q_ASSERT(!m_StdOutThreadAddress.isNull());
        Q_ASSERT(!m_StdErrThreadAddress.isNull());

        Q_ASSERT(m_pPythonMain);
        Q_ASSERT(m_pAlgsServer);
        Q_ASSERT(m_pDistrAlgsServer);
        Q_ASSERT(m_pDbInstance);
        Q_ASSERT(m_pNetServer);
        Q_ASSERT(m_pStdOutServer);
        Q_ASSERT(m_pStdErrServer);

        // state machine's high level states
        auto l_pAutoStructManagerState = new autostructmanager_State(
            m_pManagingStateMachine,
            std::make_tuple(
                m_PyThreadAddress,
                m_AlgsThreadAddress,
                m_DistrAlgsThreadAddress,
                m_DbThreadAddress,
                m_NetThreadAddress,
                m_StdOutThreadAddress,
                m_StdErrThreadAddress
            ),
            std::make_tuple(
                m_pPythonMain,
                m_pAlgsServer,
                m_pDistrAlgsServer,
                m_pDbInstance,
                m_pNetServer,
                m_pStdOutServer,
                m_pStdErrServer
            ),
            *m_upMainWindow
        );

        QObject::connect(
            l_pAutoStructManagerState,
            &autostructmanager_State::signal_vCloseThreads,
            m_pManagingStateMachine,
            &autostructmanager_StateMachine::slot_vOnCloseThreads
        );

        auto l_pQuitApplicationFinalState = new QFinalState(m_pManagingStateMachine);

        m_pManagingStateMachine->setInitialState(l_pAutoStructManagerState);

        qDebug(logDebug).noquote()
            <<  "after the autostruct manager state was set as intitial state"
        ;
        if (QThread::currentThread() != m_pManagingStateMachine->thread()) {
            qDebug(logDebug).noquote()
                <<  "m_vCreateStates() was called from a different thread than the one the state machine lives in"
            ;
        }
        else    {
            qDebug(logDebug).noquote()
                <<  "m_vCreateStates() was called from the same thread the state machine lives in"
            ;
        }

        m_upMainWindow->m_vEnableConstraintsActionsGroups(false);
        m_upMainWindow->m_vEnableActionsGroups(false);
        m_upMainWindow->m_vEnableActions();

    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vRegisterMainWindow()    {
		Q_ASSERT(m_pManagingStateMachine);
        Q_ASSERT(m_upMainWindow);

        Q_ASSERT(!m_PyThreadAddress.isNull());
        Q_ASSERT(!m_AlgsThreadAddress.isNull());
        Q_ASSERT(!m_DistrAlgsThreadAddress.isNull());
        Q_ASSERT(!m_DbThreadAddress.isNull());
        Q_ASSERT(!m_NetThreadAddress.isNull());
        Q_ASSERT(!m_StdOutThreadAddress.isNull());
        Q_ASSERT(!m_StdErrThreadAddress.isNull());

        Q_ASSERT(m_pPythonMain);
        Q_ASSERT(m_pAlgsServer);
        Q_ASSERT(m_pDistrAlgsServer);
        Q_ASSERT(m_pDbInstance);
        Q_ASSERT(m_pNetServer);
        Q_ASSERT(m_pStdOutServer);
        Q_ASSERT(m_pStdErrServer);

        m_upMainWindow->m_vRegister(
            std::make_tuple(
                m_PyThreadAddress,
                m_AlgsThreadAddress,
                m_DistrAlgsThreadAddress,
                m_DbThreadAddress,
                m_NetThreadAddress,
                m_StdOutThreadAddress,
                m_StdErrThreadAddress
            ),
            std::make_tuple(
                m_pPythonMain,
                m_pAlgsServer,
                m_pDistrAlgsServer,
                m_pDbInstance,
                m_pNetServer,
                m_pStdOutServer,
                m_pStdErrServer
            )
        );    
    }


    bool autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::mc_bIsRunning()  const   {
        QString l_qsIsStateMachineRunning;
        bool l_bSMIsRunning = m_pManagingStateMachine->isRunning();
        l_bSMIsRunning ? l_qsIsStateMachineRunning = "yes" : l_qsIsStateMachineRunning = "no"; 

        return l_bSMIsRunning;
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vMoveWorkersToThreads()  {
        Q_ASSERT(m_pqPyThread);
        Q_ASSERT(m_pqAlgsServerThread);
        Q_ASSERT(m_pqDistrAlgsServerThread);
        Q_ASSERT(m_pqDbThread);
        Q_ASSERT(m_pqNetServerThread);
        Q_ASSERT(m_pqStdOutServerThread);
        Q_ASSERT(m_pqStdErrServerThread);
        Q_ASSERT(m_pPythonMain);
        Q_ASSERT(m_pAlgsServer);
        Q_ASSERT(m_pDistrAlgsServer);
        Q_ASSERT(m_pDbInstance);
        Q_ASSERT(m_pNetServer);
        Q_ASSERT(m_pStdOutServer);
        Q_ASSERT(m_pStdErrServer);

        m_pPythonMain->moveToThread(m_pqPyThread);
        m_pAlgsServer->moveToThread(m_pqAlgsServerThread);
        m_pDistrAlgsServer->moveToThread(m_pqDistrAlgsServerThread);
        m_pDbInstance->moveToThread(m_pqDbThread);
        m_pNetServer->moveToThread(m_pqNetServerThread);
        m_pStdOutServer->moveToThread(m_pqStdOutServerThread);
        m_pStdErrServer->moveToThread(m_pqStdErrServerThread);
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vLaunchThreads() {
        Q_ASSERT(m_pqPyThread);
        Q_ASSERT(m_pqAlgsServerThread);
        Q_ASSERT(m_pqDistrAlgsServerThread);
        Q_ASSERT(m_pqDbThread);
        Q_ASSERT(m_pqNetServerThread);
        Q_ASSERT(m_pqStdOutServerThread);
        Q_ASSERT(m_pqStdErrServerThread);

        m_pqStdErrServerThread->start();
        m_pqStdOutServerThread->start();
        m_pqNetServerThread->start();
        m_pqDbThread->start();
        m_pqDistrAlgsServerThread->start();
        m_pqAlgsServerThread->start();
        m_pqPyThread->start();
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::
    m_vPlugInSignalsSlotsPyThread()    {
		Q_ASSERT(m_pManagingStateMachine);
        Q_ASSERT(m_pPythonMain);
        Q_ASSERT(m_pqPyThread);
        Q_ASSERT(m_upMainWindow);

        QObject::connect(
            m_pqPyThread,
            &QThread::started,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnThreadStarted
        );

        QObject::connect(
            m_pqPyThread,
            &QThread::finished,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnThreadFinished
        );

        QObject::connect(
            m_pqPyThread,
            &QThread::started,
            m_pPythonMain,
            &python::setup::setup_Main::slot_vOnStarted
        );

        QObject::connect(
            m_pPythonMain,
            &python::setup::setup_Main::signal_vPyThreadFinished,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnPyThreadFinished
        );

/*
        QObject::connect(
            m_pPythonMain,
            &python::setup::setup_Main::signal_vPyThreadFinished,
            m_pPythonMain,
            &python::setup::setup_Main::deleteLater);
        QObject::connect(
            m_pPythonMain,
            &python::setup::setup_Main::signal_vPyThreadFinished,
            m_pqPyThread,
            &QThread::deleteLater
        );
*/
        QObject::connect(
            m_pPythonMain,
            &python::setup::setup_Main::signal_vPyThreadError,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnPyThreadError
        );

        QObject::connect(
            m_pPythonMain,
            &python::setup::setup_Main::signal_vPyThreadError,
            m_upMainWindow.get(),
            &uiw::windows_MainWindow::slot_vOnPyThreadError
        );
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::
    m_vPlugInSignalsSlotsAlgsThread()  {
		Q_ASSERT(m_pManagingStateMachine);
        Q_ASSERT(m_pAlgsServer);
        Q_ASSERT(m_pqAlgsServerThread);
        Q_ASSERT(m_upMainWindow);

        QObject::connect(
            m_pqAlgsServerThread,
            &QThread::started,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnThreadStarted
        );

        QObject::connect(
            m_pqAlgsServerThread,
            &QThread::finished,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnThreadFinished
        );

        QObject::connect(
            m_pqAlgsServerThread,
            &QThread::started,
            m_pAlgsServer,
            &algs::strategy::strategy_Server::slot_vOnStarted
        );

        QObject::connect(
            m_pAlgsServer,
            &algs::strategy::strategy_Server::signal_vAlgsThreadFinished,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnAlgsThreadFinished
        );

/*
        QObject::connect(
            m_pAlgsServer,
            &algs::strategy::strategy_Server::signal_vAlgsThreadFinished,
            m_pAlgsServer,
            &algs::strategy::strategy_Server::deleteLater
        );
        QObject::connect(
            m_pAlgsServer,
            &algs::strategy::strategy_Server::signal_vAlgsThreadFinished,
            m_pqAlgsServerThread,
            &QThread::deleteLater
        );
*/
        QObject::connect(
            m_pAlgsServer,
            &algs::strategy::strategy_Server::signal_vAlgsThreadError,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnAlgsThreadError
        );

        QObject::connect(
            m_pAlgsServer,
            &algs::strategy::strategy_Server::signal_vAlgsThreadError,
            m_upMainWindow.get(),
            &uiw::windows_MainWindow::slot_vOnAlgsThreadError
        );
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::
    m_vPlugInSignalsSlotsDistrAlgsThread() {
		Q_ASSERT(m_pManagingStateMachine);
        Q_ASSERT(m_pDistrAlgsServer);
        Q_ASSERT(m_pqDistrAlgsServerThread);
        Q_ASSERT(m_upMainWindow);

        QObject::connect(
            m_pqDistrAlgsServerThread,
            &QThread::started,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnThreadStarted
        );

        QObject::connect(
            m_pqDistrAlgsServerThread,
            &QThread::finished,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnThreadFinished
        );

        QObject::connect(
            m_pqDistrAlgsServerThread,
            &QThread::started,
            m_pDistrAlgsServer,
            &algs::distributive::distributive_Server::slot_vOnStarted
        );

        QObject::connect(
            m_pDistrAlgsServer,
            &algs::distributive::distributive_Server::signal_vDistrAlgsThreadFinished,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnDistrAlgsThreadFinished
        );

/*
        QObject::connect(
            m_pDistrAlgsServer,
            &algs::distributive::distributive_Server::signal_vDistrAlgsThreadFinished,
            m_pDistrAlgsServer,
            &algs::distributive::distributive_Server::deleteLater
        );

        QObject::connect(
            m_pDistrAlgsServer,
            &algs::distributive::distributive_Server::signal_vDistrAlgsThreadFinished,
            m_pqDistrAlgsServerThread,
            &QThread::deleteLater
        );
*/
        QObject::connect(
            m_pDistrAlgsServer,
            &algs::distributive::distributive_Server::signal_vDistrAlgsThreadError,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnDistrAlgsThreadError
        );

        QObject::connect(
            m_pDistrAlgsServer,
            &algs::distributive::distributive_Server::signal_vDistrAlgsThreadError,
            m_upMainWindow.get(),
            &uiw::windows_MainWindow::slot_vOnDistrAlgsThreadError
        );
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::
    m_vPlugInSignalsSlotsDbInstancesThread()   {
		Q_ASSERT(m_pManagingStateMachine);
        Q_ASSERT(m_pDbInstance);
        Q_ASSERT(m_pqDbThread);
        Q_ASSERT(m_upMainWindow);

        QObject::connect(
            m_pqDbThread,
            &QThread::started,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnThreadStarted
        );

        QObject::connect(
            m_pqDbThread,
            &QThread::finished,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnThreadFinished
        );

        QObject::connect(
            m_pqDbThread,
            &QThread::started,
            m_pDbInstance,
            &db::vars::vars_DbInstance::slot_vOnStarted
        );

        QObject::connect(
            m_pDbInstance,
            &db::vars::vars_DbInstance::signal_vDbInstanceThreadFinished,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnDbInstanceThreadFinished
        );

/*
        QObject::connect(
            m_pDbInstance,
            &db::vars::vars_DbInstance::signal_vDbInstanceThreadFinished,
            m_pDbInstance,
            &db::vars::vars_DbInstance::deleteLater
        );
        QObject::connect(
            m_pDbInstance,
            &db::vars::vars_DbInstance::signal_vDbInstanceThreadFinished,
            m_pqDbThread,
            &QThread::deleteLater
        );
*/
        QObject::connect(
            m_pDbInstance,
            &db::vars::vars_DbInstance::signal_vDbInstanceThreadError,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnDbInstanceThreadError
        );

        QObject::connect(
            m_pDbInstance,
            &db::vars::vars_DbInstance::signal_vFileSaveFailure,
            m_upMainWindow.get(),
            &uiw::windows_MainWindow::slot_vOnFileSaveFailure
        );

        QObject::connect(
            m_pDbInstance,
            &db::vars::vars_DbInstance::signal_vDbInstanceThreadError,
            m_upMainWindow.get(),
            &uiw::windows_MainWindow::slot_vOnDbInstanceThreadError
        );
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::
    m_vPlugInSignalsSlotsNetThread()   {
		Q_ASSERT(m_pManagingStateMachine);
        Q_ASSERT(m_pNetServer);
        Q_ASSERT(m_pqNetServerThread);
        Q_ASSERT(m_upMainWindow);

        QObject::connect(
            m_pqNetServerThread,
            &QThread::started,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnThreadStarted
        );

        QObject::connect(
            m_pqNetServerThread,
            &QThread::finished,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnThreadFinished
        );

        QObject::connect(
            m_pqNetServerThread,
            &QThread::started,
            m_pNetServer,
            &io::network::network_Server::slot_vOnStarted
        );

        QObject::connect(
            m_pNetServer,
            &io::network::network_Server::signal_vNetThreadFinished,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnNetThreadFinished
        );

/*
        QObject::connect(
            m_pNetServer,
            &io::network::network_Server::signal_vNetThreadFinished,
            m_pNetServer,
            &io::network::network_Server::deleteLater
        );
        QObject::connect(
            m_pNetServer,
            &io::network::network_Server::signal_vNetThreadFinished,
            m_pqNetServerThread,
            &QThread::deleteLater
        );
*/
        QObject::connect(
            m_pNetServer,
            &io::network::network_Server::signal_vNetThreadError,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnNetThreadError
        );

        QObject::connect(
            m_pNetServer,
            &io::network::network_Server::signal_vNetThreadError,
            m_upMainWindow.get(),
            &uiw::windows_MainWindow::slot_vOnNetThreadError
        );
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::
    m_vPlugInSignalsSlotsStdOutThread()    {
		Q_ASSERT(m_pManagingStateMachine);
        Q_ASSERT(m_pStdOutServer);
        Q_ASSERT(m_pqStdOutServerThread);
        Q_ASSERT(m_upMainWindow);

        QObject::connect(
            m_pqStdOutServerThread,
            &QThread::started,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnThreadStarted
        );

        QObject::connect(
            m_pqStdOutServerThread,
            &QThread::finished,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnThreadFinished
        );

        QObject::connect(
            m_pqStdOutServerThread,
            &QThread::started,
            m_pStdOutServer,
            &io::stdoutput::stdoutput_Server::slot_vOnStarted
        );

        QObject::connect(
            m_pStdOutServer,
            &io::stdoutput::stdoutput_Server::signal_vStdOutThreadFinished,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnStdOutThreadFinished
        );

/*
        QObject::connect(
            m_pStdOutServer,
            &io::stdoutput::stdoutput_Server::signal_vStdOutThreadFinished,
            m_pStdOutServer,
            &io::stdoutput::stdoutput_Server::deleteLater
        );
        QObject::connect(
            m_pStdOutServer,
            &io::stdoutput::stdoutput_Server::signal_vStdOutThreadFinished,
            m_pqStdOutServerThread,
            &QThread::deleteLater
        );
*/
        QObject::connect(
            m_pStdOutServer,
            &io::stdoutput::stdoutput_Server::signal_vStdOutThreadError,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnStdOutThreadError
        );

        QObject::connect(
            m_pStdOutServer,
            &io::stdoutput::stdoutput_Server::signal_vStdOutThreadError,
            m_upMainWindow.get(),
            &uiw::windows_MainWindow::slot_vOnStdOutThreadError
        );
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::
    m_vPlugInSignalsSlotsStdErrThread()    {
		Q_ASSERT(m_pManagingStateMachine);
        Q_ASSERT(m_pStdErrServer);
        Q_ASSERT(m_pqStdErrServerThread);
        Q_ASSERT(m_upMainWindow);

        QObject::connect(
            m_pqStdErrServerThread,
            &QThread::started,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnThreadStarted
        );

        QObject::connect(
            m_pqStdErrServerThread,
            &QThread::finished,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnThreadFinished
        );

        QObject::connect(
            m_pqStdErrServerThread,
            &QThread::started,
            m_pStdErrServer,
            &io::stderror::stderror_Server::slot_vOnStarted
        );

        QObject::connect(
            m_pStdErrServer,
            &io::stderror::stderror_Server::signal_vStdErrThreadFinished,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnStdErrThreadFinished
        );

/*
        QObject::connect(
            m_pStdErrServer,
            &io::stderror::stderror_Server::signal_vStdErrThreadFinished,
            m_pStdErrServer,
            &io::stderror::stderror_Server::deleteLater
        );
        QObject::connect(
            m_pStdErrServer,
            &io::stderror::stderror_Server::signal_vStdErrThreadFinished,
            m_pqStdErrServerThread,
            &QThread::deleteLater
        );
*/
        QObject::connect(
            m_pStdErrServer,
            &io::stderror::stderror_Server::signal_vStdErrThreadError,
            m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnStdOutThreadError
        );

        QObject::connect(
            m_pStdErrServer,
            &io::stderror::stderror_Server::signal_vStdErrThreadError,
            m_upMainWindow.get(),
            &uiw::windows_MainWindow::slot_vOnStdErrThreadError
        );
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vGetAddresses()  {
		Q_ASSERT(m_pManagingStateMachine);
        Q_ASSERT(m_pPythonMain);
        Q_ASSERT(m_pAlgsServer);
        Q_ASSERT(m_pDistrAlgsServer);
        Q_ASSERT(m_pDbInstance);
        Q_ASSERT(m_pNetServer);
        Q_ASSERT(m_pStdOutServer);
        Q_ASSERT(m_pStdErrServer);

        QObject::connect(
            m_pPythonMain,
            &python::setup::setup_Main::signal_vMailBoxCreated,
		    m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnPyThreadMailBoxCreated
        );

        QObject::connect(
            m_pAlgsServer,
            &algs::strategy::strategy_Server::signal_vMailBoxCreated,
		    m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnAlgsThreadMailBoxCreated
        );

        QObject::connect(
            m_pDistrAlgsServer,
            &algs::distributive::distributive_Server::signal_vMailBoxCreated,
		    m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnDistrAlgsThreadMailBoxCreated
        );

        QObject::connect(
            m_pDbInstance,
            &db::vars::vars_DbInstance::signal_vMailBoxCreated,
		    m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnDbThreadMailBoxCreated
        );

        QObject::connect(
            m_pNetServer,
            &io::network::network_Server::signal_vMailBoxCreated,
		    m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnNetThreadMailBoxCreated
        );

        QObject::connect(
            m_pStdOutServer,
            &io::stdoutput::stdoutput_Server::signal_vMailBoxCreated,
		    m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnStdOutThreadMailBoxCreated
        );

        QObject::connect(
            m_pStdErrServer,
            &io::stderror::stderror_Server::signal_vMailBoxCreated,
		    m_pManagingStateMachine,
            &states::autostructmanager::autostructmanager_StateMachine::slot_vOnStdErrThreadMailBoxCreated
        );
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vRegisterAddresses() {
        Q_ASSERT(m_pPythonMain);
        Q_ASSERT(m_pAlgsServer);
        Q_ASSERT(m_pDistrAlgsServer);
        Q_ASSERT(m_pDbInstance);
        Q_ASSERT(m_pNetServer);
        Q_ASSERT(m_pStdOutServer);
        Q_ASSERT(m_pStdErrServer);
        Q_ASSERT(!m_PyThreadAddress.isNull());
        Q_ASSERT(!m_AlgsThreadAddress.isNull());
        Q_ASSERT(!m_DistrAlgsThreadAddress.isNull());
        Q_ASSERT(!m_DbThreadAddress.isNull());
        Q_ASSERT(!m_NetThreadAddress.isNull());
        Q_ASSERT(!m_StdOutThreadAddress.isNull());
        Q_ASSERT(!m_StdErrThreadAddress.isNull());

        m_pPythonMain->m_vRegister(
            std::make_tuple(
                m_AlgsThreadAddress,
                m_DistrAlgsThreadAddress,
                m_DbThreadAddress,
                m_NetThreadAddress,
                m_StdOutThreadAddress,
                m_StdErrThreadAddress
            )
        );

        m_pAlgsServer->m_vRegister(
            std::make_tuple(
                m_PyThreadAddress,
                m_DistrAlgsThreadAddress,
                m_DbThreadAddress,
                m_NetThreadAddress,
                m_StdOutThreadAddress,
                m_StdErrThreadAddress
            )
        );


        m_pDistrAlgsServer->m_vRegister(
            std::make_tuple(
                m_PyThreadAddress,
                m_AlgsThreadAddress,
                m_DbThreadAddress,
                m_NetThreadAddress,
                m_StdOutThreadAddress,
                m_StdErrThreadAddress
            )
        );

        m_pDbInstance->m_vRegister(
            std::make_tuple(
                m_PyThreadAddress,
                m_AlgsThreadAddress,
                m_DistrAlgsThreadAddress,
                m_NetThreadAddress,
                m_StdOutThreadAddress,
                m_StdErrThreadAddress
            )
        );

        m_pNetServer->m_vRegister(
            std::make_tuple(
                m_PyThreadAddress,
                m_AlgsThreadAddress,
                m_DistrAlgsThreadAddress,
                m_DbThreadAddress,
                m_StdOutThreadAddress,
                m_StdErrThreadAddress
            )
        );

        m_pStdOutServer->m_vRegister(
            std::make_tuple(
                m_PyThreadAddress,
                m_AlgsThreadAddress,
                m_DistrAlgsThreadAddress,
                m_DbThreadAddress,
                m_NetThreadAddress,
                m_StdErrThreadAddress
            )
        );

        m_pStdErrServer->m_vRegister(
            std::make_tuple(
                m_PyThreadAddress,
                m_AlgsThreadAddress,
                m_DistrAlgsThreadAddress,
                m_DbThreadAddress,
                m_NetThreadAddress,
                m_StdOutThreadAddress
            )
        );
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vRegisterMetaTypes() {
        qRegisterMetaType<threads::registry::registry_MFCommandLineSender::type>(
            "threads::registry::registry_MFCommandLineSender::type"
        );

        qRegisterMetaType<threads::registry::registry_MFAlgsSender::type>(
            "threads::registry::registry_MFAlgsSender::type"
        );

        qRegisterMetaType<threads::registry::registry_MFDistrAlgsSender::type>(
            "threads::registry::registry_MFDistrAlgsSender::type"
        );

        qRegisterMetaType<threads::registry::registry_MFDbInstanceSender::type>(
            "threads::registry::registry_MFDbInstanceSender::type"
        );

        qRegisterMetaType<threads::registry::registry_MFNetSender::type>(
            "threads::registry::registry_MFNetSender::type"
        );

        qRegisterMetaType<threads::registry::registry_MFStdOutSender::type>(
            "threads::registry::registry_MFStdOutSender::type"
        );

        qRegisterMetaType<threads::registry::registry_MFStdErrSender::type>(
            "threads::registry::registry_MFStdErrSender::type"
        );
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vDisplayPyThreadDlg(
        QJsonObject const& airc_qjoPyThreadError
    )    {
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vDisplayAlgsThreadDlg(
        QJsonObject const& airc_qjoAlgsThreadError
    )    {
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vDisplayDistrAlgsThreadDlg(
        QJsonObject const& airc_qjoDistrAlgsThreadError
    )  {
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vDisplayDbInstanceThreadDlg(
        QJsonObject const& airc_qjoDbInstanceThreadError
    )    {
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vDisplayNetThreadDlg(
        QJsonObject const& airc_qjoNetThreadError
    )  {
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vDisplayStdOutThreadDlg(
        QJsonObject const& airc_qjoStdOutThreadError
    )    {
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vDisplayStdErrThreadDlg(
        QJsonObject const& airc_qjoStdErrThreadError
    )    {
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vSendErrorToStdErrThread(
        QJsonObject const& airc_qjoPyThreadError
    )   {
    }

    void autostructmanager_StateMachine::autostructmanager_StateMachineImpl01::m_vSendErrorToWebServer(
        QJsonObject const& airc_qjoPyThreadError
    )  {
    }

/////////////autostructmanager_StateMachine/////////////

// constructors
autostructmanager_StateMachine::autostructmanager_StateMachine(
    QObject* ai_pParent
)
	:	QStateMachine(ai_pParent)
{
	m_spStateMachineImpl.reset(new autostructmanager_StateMachineImpl01(this));
}

autostructmanager_StateMachine::~autostructmanager_StateMachine()	{
    qDebug(logDebug).noquote()
        <<  "state machine is going to die"
    ;
}

// operations

// operation slots
void autostructmanager_StateMachine::slot_vStart()  {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vStartStateMachine();
}

void autostructmanager_StateMachine::slot_vOnCloseThreads()  {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vOnCloseThreads();
}

// overriding functionalities
void autostructmanager_StateMachine::onEntry(QEvent* ai_pEvent) {
	Q_ASSERT(m_spStateMachineImpl);
    Q_ASSERT(ai_pEvent);

    m_spStateMachineImpl->m_vOnEntry(ai_pEvent);
    QStateMachine::onEntry(ai_pEvent);
}

void autostructmanager_StateMachine::onExit(QEvent* ai_pEvent) {
	Q_ASSERT(m_spStateMachineImpl);
    Q_ASSERT(ai_pEvent);

    m_spStateMachineImpl->m_vOnExit(ai_pEvent);
    QStateMachine::onExit(ai_pEvent);
}

// private delegates
void autostructmanager_StateMachine::slot_vOnThreadStarted() {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vThreadStarted();
}

void autostructmanager_StateMachine::slot_vOnThreadFinished() {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vThreadFinished();
}

void autostructmanager_StateMachine::slot_vOnPyThreadFinished() {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vPyThreadFinished();
}

void autostructmanager_StateMachine::slot_vOnPyThreadError(QJsonObject const& airc_qjoPyThreadError) {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vPyThreadError(airc_qjoPyThreadError);
}

void autostructmanager_StateMachine::slot_vOnAlgsThreadFinished()   {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vAlgsThreadFinished();
}

void autostructmanager_StateMachine::slot_vOnAlgsThreadError(QJsonObject const& airc_qjoAlgsThreadError)   {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vAlgsThreadError(airc_qjoAlgsThreadError);
}

void autostructmanager_StateMachine::slot_vOnDistrAlgsThreadFinished()  {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vDistrAlgsThreadFinished();
}

void autostructmanager_StateMachine::slot_vOnDistrAlgsThreadError(
    QJsonObject const& airc_qjoDistrAlgsThreadError
)  {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vDistrAlgsThreadError(airc_qjoDistrAlgsThreadError);
}

void autostructmanager_StateMachine::slot_vOnDbInstanceThreadFinished() {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vDbInstanceThreadFinished();
}

void autostructmanager_StateMachine::slot_vOnDbInstanceThreadError(QJsonObject const& airc_qjoDbThreadError) {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vDbInstanceThreadError(airc_qjoDbThreadError);
}

void autostructmanager_StateMachine::slot_vOnNetThreadFinished()    {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vNetThreadFinished();
}

void autostructmanager_StateMachine::slot_vOnNetThreadError(QJsonObject const& airc_qjoNetThreadError)    {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vNetThreadError(airc_qjoNetThreadError);
}

void autostructmanager_StateMachine::slot_vOnStdOutThreadFinished() {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vStdOutThreadFinished();
}

void autostructmanager_StateMachine::slot_vOnStdOutThreadError(QJsonObject const& airc_qjoStdOutThreadError) {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vStdOutThreadError(airc_qjoStdOutThreadError);
}

void autostructmanager_StateMachine::slot_vOnStdErrThreadFinished() {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vStdErrThreadFinished();
}

void autostructmanager_StateMachine::slot_vOnStdErrThreadError(QJsonObject const& airc_qjoStdErrThreadError) {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vStdErrThreadError(airc_qjoStdErrThreadError);
}

void autostructmanager_StateMachine::autostructmanager_StateMachine::slot_vOnPyThreadMailBoxCreated(
    threads::registry::registry_MFCommandLineSender::type ai_cMailBox
)   {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vPyThreadMailBoxCreated(ai_cMailBox);
}

void autostructmanager_StateMachine::autostructmanager_StateMachine::slot_vOnAlgsThreadMailBoxCreated(
    threads::registry::registry_MFAlgsSender::type ai_cMailBox
)   {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vAlgsThreadMailBoxCreated(ai_cMailBox);
}

void autostructmanager_StateMachine::autostructmanager_StateMachine::slot_vOnDistrAlgsThreadMailBoxCreated(
    threads::registry::registry_MFDistrAlgsSender::type ai_cMailBox
)   {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vDistrAlgsThreadMailBoxCreated(ai_cMailBox);
}

void autostructmanager_StateMachine::autostructmanager_StateMachine::slot_vOnDbThreadMailBoxCreated(
    threads::registry::registry_MFDbInstanceSender::type ai_cMailBox
)  {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vDbThreadMailBoxCreated(ai_cMailBox);
}

void autostructmanager_StateMachine::autostructmanager_StateMachine::slot_vOnNetThreadMailBoxCreated(
    threads::registry::registry_MFNetSender::type ai_cMailBox
) {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vNetThreadMailBoxCreated(ai_cMailBox);
}

void autostructmanager_StateMachine::autostructmanager_StateMachine::slot_vOnStdOutThreadMailBoxCreated(
    threads::registry::registry_MFStdOutSender::type ai_cMailBox
)  {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vStdOutThreadMailBoxCreated(ai_cMailBox);
}

void autostructmanager_StateMachine::autostructmanager_StateMachine::slot_vOnStdErrThreadMailBoxCreated(
    threads::registry::registry_MFStdErrSender::type ai_cMailBox
)  {
	Q_ASSERT(m_spStateMachineImpl);

    m_spStateMachineImpl->m_vStdErrThreadMailBoxCreated(ai_cMailBox);
}


	}	// namespace autostructmanager
}	// namespace states
