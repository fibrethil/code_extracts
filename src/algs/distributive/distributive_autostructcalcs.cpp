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

#include "distributive_autostructcalcs.h"

#include <algs/include/local/distrmsgs_launchprocessevent.h>
#include <algs/include/local/distrmsgs_processoutputevent.h>
#include <algs/include/local/distrmsgs_processcanceledevent.h>
#include <algs/include/local/distrmsgs_processprogressevent.h>
#include <algs/include/local/distrmsgs_processerrorevent.h>
#include <algs/include/local/distrmsgs_processstartedevent.h>
#include <algs/include/local/distrmsgs_processfinishedevent.h>
#include <config/include/storey_slabs.h>
#include <io/include/stdoutmsgs_locallogstate.h>
#include <io/include/stdoutmsgs_localprocessstarted.h>
#include <specs/include/applications_autostructcalcs.h>
#include <specs/include/keys_stathmi.h>
#include <specs/include/errors_critical.h>
#include <tools/include/logger_categories.h>

#include <QtGlobal>
#include <QtDebug>
#include <QUndoCommand>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QStringList>
#include <QString>
#include <QByteArray>

#include <algorithm>

	namespace /* unit */	{

	/*********************************************
	*
	* constants
	*
	**********************************************/
    char const* uc_pcArguments = "arguments";

    char const* uc_pcLevel = "level";
    char const* uc_pcProgress = "progress";
    char const* uc_pcError = "error";
    char const* uc_pcExitCode = "exitCode";


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
	* interface distributive_AutoStructCalcsImpl01
	*
	**********************************************/

	class distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01		{
		// data
		private:
			distributive_AutoStructCalcs* m_pAutoStructCalcs;

            QString m_qsApplication;
            QStringList m_qslArguments;
            QJsonObject m_qjoProcessInput;

            QProcess* m_pqProcess   { nullptr };
            QUndoCommand* m_pqCommand   {nullptr};

            bool m_bFileCommunication   {true};

            threads::registry::registry_MFCommandLineSender::type m_PyThreadAddress;
            threads::registry::registry_MFAlgsSender::type m_AlgsThreadAddress;
            threads::registry::registry_MFAlgsSender::type m_DistrAlgsThreadAddress;
            threads::registry::registry_MFDbInstanceSender::type m_DbThreadAddress;
            threads::registry::registry_MFNetSender::type m_NetThreadAddress;
            threads::registry::registry_MFStdOutSender::type m_StdOutThreadAddress;
            threads::registry::registry_MFStdErrSender::type m_StdErrThreadAddress;

        // helper functions
        private:
            void m_vSendProcessStartedLogMessage(QStringList const& airc_qslArguments);
            void m_vPluginSignalsSlots();
            void m_vProcessFailedToStart();
            void m_vProcessCrashed();
            void m_vProcessTimeout();
            void m_vProcessWriteError();
            void m_vProcessReadError();
            void m_vProcessUnknownError();
            void m_vNormalExit(int ai_iExitCode);
            void m_vCrashedExit(int ai_iExitCode);

            bool m_bWriteCommunicationFile(QString const& airc_qsJsonCommunicationFile, QByteArray const& airc_qbaLevelJsonObject);
	
		// constructors
		public:
			distributive_AutoStructCalcsImpl01() = default;
			distributive_AutoStructCalcsImpl01(
                distributive_AutoStructCalcs*,
                QString const& airc_qsApplication,
                QStringList const& airc_qslArguments,
                QJsonObject const& airc_qjoProcessInput,
                QUndoCommand* ai_pqCommand
            );
			distributive_AutoStructCalcsImpl01(distributive_AutoStructCalcsImpl01 const&) = default;
			distributive_AutoStructCalcsImpl01(distributive_AutoStructCalcsImpl01&&) = default;
			~distributive_AutoStructCalcsImpl01();


		// operators
		public:
			distributive_AutoStructCalcsImpl01& operator = (
                distributive_AutoStructCalcsImpl01 const&
            ) = default;
			distributive_AutoStructCalcsImpl01& operator = (
                distributive_AutoStructCalcsImpl01&&
            ) = default;

        // operations
        public:
            void m_vRegister(
                std::tuple<
                    threads::registry::registry_MFCommandLineSender::type,
                    threads::registry::registry_MFAlgsSender::type,
                    threads::registry::registry_MFDistrAlgsSender::type,
                    threads::registry::registry_MFDbInstanceSender::type,
                    threads::registry::registry_MFNetSender::type,
                    threads::registry::registry_MFStdOutSender::type,
                    threads::registry::registry_MFStdErrSender::type
                > const&
            );
            void m_vCancel();

        // delegates
        public:
            void m_vOnExecuteProgram();
            void m_vOnErrorOccured(QProcess::ProcessError);
            void m_vOnCancelProcess(QString const& airc_qsApplication, QUndoCommand* ai_pqCommand);
            void m_vOnStarted();
            void m_vOnFinished(int ai_iExitCode, QProcess::ExitStatus ai_eExitStatus);
            void m_vOnReadyReadStandardOutput();
            void m_vOnReadyReadStandardError();
		

	};

	/*********************************************
	*
	* implementation distributive_AutoStructCalcsImpl01	
	*
	**********************************************/
	// constructors
	distributive_AutoStructCalcs::
    distributive_AutoStructCalcsImpl01::distributive_AutoStructCalcsImpl01(
        distributive_AutoStructCalcs* ai_pAutoStructCalcs,
        QString const& airc_qsApplication,
        QStringList const& airc_qslArguments,
        QJsonObject const& airc_qjoProcessInput,
        QUndoCommand* ai_pqCommand
    )
		:	m_pAutoStructCalcs(ai_pAutoStructCalcs)
        ,   m_qsApplication(airc_qsApplication)
        ,   m_qslArguments(airc_qslArguments)
        ,   m_pqProcess(new QProcess(m_pAutoStructCalcs))
        ,   m_qjoProcessInput(airc_qjoProcessInput)
        ,   m_pqCommand(ai_pqCommand)
	{
		Q_ASSERT(m_pAutoStructCalcs);
        Q_ASSERT(!m_qsApplication.isNull());
        Q_ASSERT(m_pqCommand);

        m_vPluginSignalsSlots();
	}

	distributive_AutoStructCalcs::
    distributive_AutoStructCalcsImpl01::~distributive_AutoStructCalcsImpl01()    {
        Q_ASSERT(m_pqProcess);

        m_pqProcess->kill();
    }


    // operations
    void distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01::m_vRegister(
        std::tuple<
            threads::registry::registry_MFCommandLineSender::type,
            threads::registry::registry_MFAlgsSender::type,
            threads::registry::registry_MFDistrAlgsSender::type,
            threads::registry::registry_MFDbInstanceSender::type,
            threads::registry::registry_MFNetSender::type,
            threads::registry::registry_MFStdOutSender::type,
            threads::registry::registry_MFStdErrSender::type
        > const& airc_tupleThreadAddresses
    )   {
        auto [
            l_PyThreadAddress,
            l_AlgsThreadAddress,
            l_DistrAlgsThreadAddress,
            l_DbThreadAddress,
            l_NetThreadAddress,
            l_StdOutThreadAddress, 
            l_StdErrThreadAddress 
        ] = airc_tupleThreadAddresses;
        m_PyThreadAddress = l_PyThreadAddress; 
        m_AlgsThreadAddress = l_AlgsThreadAddress; 
        m_DistrAlgsThreadAddress = l_DistrAlgsThreadAddress; 
        m_DbThreadAddress = l_DbThreadAddress; 
        m_NetThreadAddress = l_NetThreadAddress; 
        m_StdOutThreadAddress = l_StdOutThreadAddress; 
        m_StdErrThreadAddress = l_StdErrThreadAddress; 

        Q_ASSERT(!m_PyThreadAddress.isNull());
        Q_ASSERT(!m_AlgsThreadAddress.isNull());
        Q_ASSERT(!m_DistrAlgsThreadAddress.isNull());
        Q_ASSERT(!m_DbThreadAddress.isNull());
        Q_ASSERT(!m_NetThreadAddress.isNull());
        Q_ASSERT(!m_StdOutThreadAddress.isNull());
        Q_ASSERT(!m_StdErrThreadAddress.isNull());
    }

    void distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01::m_vCancel()    {
        // there is no need to kill the process on cancelation
        // as it is the thread is going to be destructed, so
        // its destructor will kill the process
    }

    // delegates
    void distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01::m_vOnExecuteProgram()  {
        Q_ASSERT(m_pqProcess);
        Q_ASSERT(!m_qsApplication.isNull());
        Q_ASSERT(26 == m_qslArguments.size());

        QJsonDocument l_qjdProcessInput(m_qjoProcessInput);
        QByteArray l_qbaProcessInput = l_qjdProcessInput.toJson();

        bool l_bWriteToProcess  {false};
        QStringList l_qslArguments(m_qslArguments.begin(), m_qslArguments.begin() + 10);
//        std::copy_n(m_qslArguments.begin(), 10, l_qslArguments.begin());
        if (m_bFileCommunication)  {

            auto l_qsCommunicationFile = m_qslArguments[5];

            qDebug(logDebug).noquote()
                <<  "l_qsCommunicationFile is : " + l_qsCommunicationFile
                <<  Qt::endl
            ;

            l_bWriteToProcess = !m_bWriteCommunicationFile(l_qsCommunicationFile, l_qbaProcessInput);
            if (l_bWriteToProcess) {
                Q_ASSERT(!l_qslArguments.empty());

                l_qslArguments.pop_back();
                l_qslArguments.pop_back();
            }
        }
        qDebug(logDebug).noquote()
            <<  "the programs we are going to launch is: " + m_qsApplication
            <<  "the arguments are: " + l_qslArguments.join(" ")
        ;

        m_pqProcess->start(
            m_qsApplication,
            l_qslArguments
        );

        m_vSendProcessStartedLogMessage(m_qslArguments);

        if (l_bWriteToProcess)  {

            m_pqProcess->write(l_qbaProcessInput.constData());
        }

        m_pqProcess->waitForStarted();
    }

    void distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01::m_vOnErrorOccured(
        QProcess::ProcessError ai_eProcessError
    )  {
		Q_ASSERT(m_pAutoStructCalcs);

        switch(ai_eProcessError)    {
            case QProcess::FailedToStart:
                m_vProcessFailedToStart();
                break;
            case QProcess::Crashed:
                m_vProcessCrashed();
                break;
            case QProcess::Timedout:
                m_vProcessTimeout();
                break;
            case QProcess::WriteError:
                m_vProcessWriteError();
                break;
            case QProcess::ReadError:
                m_vProcessReadError();
                break;
            case QProcess::UnknownError:
                m_vProcessUnknownError();
                break;
        }
    }

    void distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01::m_vOnCancelProcess(
        QString const& airc_qsApplication,
        QUndoCommand* ai_pqCommand
    )   {
        Q_ASSERT(!airc_qsApplication.isNull());
        Q_ASSERT(ai_pqCommand);

        m_DistrAlgsThreadAddress.send(
            algs::distrmsgs::distrmsgs_ProcessCanceledEvent(
                airc_qsApplication,
                ai_pqCommand
            )
        );
    }

    void distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01::m_vOnStarted()  {
        Q_ASSERT(!m_qsApplication.isNull());
        Q_ASSERT(m_pqCommand);

        QJsonObject l_qjoProcessStarted;
        QJsonArray l_qjrArguments;
        for (auto l_qsArgument : m_qslArguments)    {
            l_qjrArguments.push_back(l_qsArgument);
        }

        l_qjoProcessStarted.insert(
            uc_pcArguments,
            l_qjrArguments
        );

        m_DistrAlgsThreadAddress.send(
            distrmsgs::distrmsgs_ProcessStartedEvent(
                m_qsApplication,
                l_qjoProcessStarted,
                m_pqCommand
            )
        );
    }

    void distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01::m_vOnFinished(
        int ai_iExitCode,
        QProcess::ExitStatus ai_eExitStatus
    )   {
		Q_ASSERT(m_pAutoStructCalcs);

        if (QProcess::NormalExit == ai_eExitStatus)
            m_vNormalExit(ai_iExitCode)
        ;
        else if (QProcess::CrashExit == ai_eExitStatus)
            m_vCrashedExit(ai_iExitCode)
        ;
    }

    void distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01::m_vOnReadyReadStandardOutput()  {
		Q_ASSERT(m_pAutoStructCalcs);
        Q_ASSERT(m_pqProcess);
        Q_ASSERT(m_pqCommand);

        auto l_qbaProcessOutput = m_pqProcess->readAllStandardOutput();
        qDebug(logDebug).noquote()
            <<  "the programs we've launched responded: " + m_qsApplication
            <<  l_qbaProcessOutput
        ;
/*
        QJsonDocument l_qjdProcessOutput = QJsonDocument::fromJson(l_qbaProcessOutput);
        QJsonObject l_qjoProcessOutput = l_qjdProcessOutput.object();

        auto l_qitrcProcessProgress = l_qjoProcessOutput.constFind(uc_pcProgress);
        Q_ASSERT(l_qjoProcessOutput.constEnd() != l_qitrcProcessProgress);

        auto l_qjvProcessProgress = l_qitrcProcessProgress.value();
        Q_ASSERT(l_qjvProcessProgress.isDouble());

        auto l_dProcessProgress = l_qjvProcessProgress.toDouble();

        if (100 != std::round(l_dProcessProgress))    {
            m_DistrAlgsThreadAddress.send(
                distrmsgs::distrmsgs_ProcessProgressEvent(
                    m_qsApplication,
                    l_dProcessProgress,
                    m_pqCommand
                )
            );
        }
        else    {
            m_DistrAlgsThreadAddress.send(
                distrmsgs::distrmsgs_ProcessOutputEvent(
                    m_qsApplication,
                    l_qjoProcessOutput,
                    m_pqCommand
                )
            );
        }
*/
    }

    void distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01::
    m_vOnReadyReadStandardError()  {
		Q_ASSERT(m_pAutoStructCalcs);
        Q_ASSERT(m_pqProcess);
        Q_ASSERT(!m_qsApplication.isNull());
        Q_ASSERT(m_pqCommand);

        QJsonObject l_qjoProcessApplicationError;
        l_qjoProcessApplicationError.insert(
            uc_pcError,
            QObject::tr("Application: ") + m_qsApplication + QObject::tr(" reported the following error: ") + m_pqProcess->readAllStandardError()
        );
        auto l_iExitCode = -1;  // application error
        l_qjoProcessApplicationError.insert(uc_pcExitCode, l_iExitCode);

        m_DistrAlgsThreadAddress.send(
            distrmsgs::distrmsgs_ProcessErrorEvent(
                m_qsApplication,
                l_qjoProcessApplicationError,
                m_pqCommand
            )
        );
    }

    // helper functions
    void distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01:: m_vPluginSignalsSlots() {
		Q_ASSERT(m_pAutoStructCalcs);
        Q_ASSERT(m_pqProcess);

        QObject::connect(
            m_pqProcess,
//            QOverload<int, QProcess::ExitStatus>::of(&QProcess::started),
            &QProcess::started,
            m_pAutoStructCalcs,
            &distributive_AutoStructCalcs::slot_vOnStarted
        );

        QObject::connect(
            m_pqProcess,
            &QProcess::errorOccurred,
            m_pAutoStructCalcs,
            &distributive_AutoStructCalcs::slot_vOnErrorOccured
        );
        QObject::connect(
            m_pqProcess,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            m_pAutoStructCalcs,
            &distributive_AutoStructCalcs::slot_vOnFinished
        );
        QObject::connect(
            m_pqProcess,
            &QProcess::readyReadStandardError,
            m_pAutoStructCalcs,
            &distributive_AutoStructCalcs::slot_vOnReadyReadStandardError
        );

        QObject::connect(
            m_pqProcess,
            &QProcess::readyReadStandardOutput,
            m_pAutoStructCalcs,
            &distributive_AutoStructCalcs::slot_vOnReadyReadStandardOutput
        );
    }

    void distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01::
    m_vProcessFailedToStart()	{
        Q_ASSERT(!m_qsApplication.isNull());
        Q_ASSERT(m_pqCommand);

        QJsonObject l_qjoProcessFailedToStart;
        l_qjoProcessFailedToStart.insert(
            uc_pcError,
            QObject::tr("Application: ") + m_qsApplication + QObject::tr("failed to Start")
        );
        auto l_iExitCode = -2;  // application failed to start error
        l_qjoProcessFailedToStart.insert(uc_pcExitCode, l_iExitCode);

        m_DistrAlgsThreadAddress.send(
            distrmsgs::distrmsgs_ProcessErrorEvent(
                m_qsApplication,
                l_qjoProcessFailedToStart,
                m_pqCommand
            )
        );
        
    }

    void distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01::m_vProcessCrashed()	{
        Q_ASSERT(!m_qsApplication.isNull());
        Q_ASSERT(m_pqCommand);

        QJsonObject l_qjoProcessCrashed;
        l_qjoProcessCrashed.insert(
            uc_pcError,
            QObject::tr("Application: ") + m_qsApplication + QObject::tr("crashed")
        );
        auto l_iExitCode = -3;  // process crashed error
        l_qjoProcessCrashed.insert(uc_pcExitCode, l_iExitCode);

        m_DistrAlgsThreadAddress.send(
            distrmsgs::distrmsgs_ProcessErrorEvent(
                m_qsApplication,
                l_qjoProcessCrashed,
                m_pqCommand
            )
        );
    }

    void distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01::m_vProcessTimeout()	{
        Q_ASSERT(!m_qsApplication.isNull());
        Q_ASSERT(m_pqCommand);

        QJsonObject l_qjoProcessTimedOut;
        l_qjoProcessTimedOut.insert(
            uc_pcError,
            QObject::tr("Application: ") + m_qsApplication + QObject::tr("timed out")
        );
        auto l_iExitCode = -4;  // process timeout error
        l_qjoProcessTimedOut.insert(uc_pcExitCode, l_iExitCode);

        m_DistrAlgsThreadAddress.send(
            distrmsgs::distrmsgs_ProcessErrorEvent(
                m_qsApplication,
                l_qjoProcessTimedOut,
                m_pqCommand
            )
        );
    }

    void distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01::
    m_vProcessWriteError()	{
        Q_ASSERT(!m_qsApplication.isNull());
        Q_ASSERT(m_pqCommand);

        QJsonObject l_qjoProcessWriteError;
        l_qjoProcessWriteError.insert(
            uc_pcError,
            QObject::tr("Application: ") + m_qsApplication + QObject::tr("could transmit back data")
        );
        auto l_iExitCode = -5;  // process write error
        l_qjoProcessWriteError.insert(uc_pcExitCode, l_iExitCode);

        m_DistrAlgsThreadAddress.send(
            distrmsgs::distrmsgs_ProcessErrorEvent(
                m_qsApplication,
                l_qjoProcessWriteError,
                m_pqCommand
            )
        );
    }

    void distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01::
    m_vProcessReadError()	{
        Q_ASSERT(!m_qsApplication.isNull());
        Q_ASSERT(m_pqCommand);

        QJsonObject l_qjoProcessReadError;
        l_qjoProcessReadError.insert(
            uc_pcError,
            QObject::tr("We could not transmit data to application: ") + m_qsApplication
        );
        auto l_iExitCode = -6;  // process read error
        l_qjoProcessReadError.insert(uc_pcExitCode, l_iExitCode);

        m_DistrAlgsThreadAddress.send(
            distrmsgs::distrmsgs_ProcessErrorEvent(
                m_qsApplication,
                l_qjoProcessReadError,
                m_pqCommand
            )
        );
    }

    void distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01::
    m_vProcessUnknownError()	{
        Q_ASSERT(!m_qsApplication.isNull());
        Q_ASSERT(m_pqCommand);

        QJsonObject l_qjoProcessUnknownError;
        l_qjoProcessUnknownError.insert(
            uc_pcError,
            QObject::tr("The process for the application: ") + m_qsApplication + QObject::tr(" signaled for an uknown error")
        );
        auto l_iExitCode = -7;  // process unknown error
        l_qjoProcessUnknownError.insert(uc_pcExitCode, l_iExitCode);

        m_DistrAlgsThreadAddress.send(
            distrmsgs::distrmsgs_ProcessErrorEvent(
                m_qsApplication,
                l_qjoProcessUnknownError,
                m_pqCommand
            )
        );
    }

    void distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01::m_vNormalExit(
        int ai_iExitCode
    )	{
        Q_ASSERT(m_pAutoStructCalcs);
        Q_ASSERT(!m_qsApplication.isNull());
        Q_ASSERT(m_pqCommand);
        Q_ASSERT(26 == m_qslArguments.size());

        auto l_qsJsonOutput = m_qslArguments[7];
        Q_ASSERT(!l_qsJsonOutput.isEmpty());

        QFile l_qfJsonOutput(l_qsJsonOutput);
        QJsonObject l_qjoProcessOutput;
        bool l_bLevelJsonOutput = false;
        if (!l_qfJsonOutput.open(QIODevice::ReadOnly | QIODevice::Text))    {
            l_qjoProcessOutput.insert(
                uc_pcError,
                QObject::tr("Application: ") + m_qsApplication + QObject::tr("crashed")
            );
        }
        else    {
            auto l_qbaJsonOutput = l_qfJsonOutput.readAll();
            QJsonDocument l_qjdProcessOutput = QJsonDocument::fromJson(l_qbaJsonOutput/*.toUtf8()*/);
            l_qjoProcessOutput = l_qjdProcessOutput.object();

            l_bLevelJsonOutput = true;
        }

        l_qfJsonOutput.close();

        l_qjoProcessOutput.insert(uc_pcExitCode, ai_iExitCode);
        m_DistrAlgsThreadAddress.send(
            distrmsgs::distrmsgs_ProcessOutputEvent(
                m_qsApplication,
                l_qjoProcessOutput,
                m_pqCommand
            )
        );

        m_DistrAlgsThreadAddress.send(
            distrmsgs::distrmsgs_ProcessFinishedEvent(
                m_qsApplication,
                l_qjoProcessOutput,
                m_pqCommand
            )
        );

        if (l_bLevelJsonOutput) {
            auto l_qsAutoStructRc = m_qslArguments[11];
            auto l_qsAutoStructLogFile = m_qslArguments[1];
            auto l_qsCommand = m_qslArguments[3];
            auto l_qsLevel = m_qslArguments[7];
            auto l_qsLevelCalcs = m_qslArguments[13];
            auto l_qsLevelRc = m_qslArguments[14];
            auto l_qsAutoStructRcConfigFile = m_qslArguments[12];
            auto l_qsContBeams = m_qslArguments[15];
            auto l_qsContSlabs = m_qslArguments[16];
            auto l_qsCodeOfPractice = m_qslArguments[17];
            auto l_qsSlabsLoadDistributionModel = m_qslArguments[18];
            auto l_qsSlabsSupportsUseAlternativeLoadings = m_qslArguments[19];
            auto l_qsContinuousSlabsSolverExec = m_qslArguments[20];
            auto l_qsContinuousSlabs = m_qslArguments[21];
            auto l_qsSpanningSlabs = m_qslArguments[22];
            auto l_qsLevelDir = m_qslArguments[23];
            auto l_qsContSpanSlabs = m_qslArguments[24];
            auto l_qsContSpanSlabsSS = m_qslArguments[25];
            auto l_ConfigSlabs = config::storey::storey_Slabs(
                    l_qsAutoStructLogFile,
                    l_qsCommand,
                    l_qsLevel,
                    l_qsLevelCalcs,
                    l_qsLevelRc,
                    l_qsContBeams,
                    l_qsContSlabs,
                    l_qsCodeOfPractice,
                    l_qsSlabsLoadDistributionModel,
                    l_qsSlabsSupportsUseAlternativeLoadings,
                    l_qsContinuousSlabsSolverExec,
                    l_qsContinuousSlabs,
                    l_qsSpanningSlabs,
                    l_qsLevelDir,
                    l_qsContSpanSlabs,
                    l_qsContSpanSlabsSS
                )
            ;
            auto l_qjoConfigSlabs = l_ConfigSlabs.mc_qjoToJson();
//          Q_EMIT is not working because the autostructcalcsmanager thread has its qt event loop frozen
//          due to the threads state machine's event loop's existence
//            Q_EMIT m_pAutoStructCalcs->signal_vLaunchAutoStructRc(
            m_DistrAlgsThreadAddress.send(
                distrmsgs::distrmsgs_LaunchProcessEvent(
                    l_qsAutoStructRc,
                    m_qslArguments,
                    l_qjoConfigSlabs,
                    m_pqCommand
                )
            );
        }

    }

    void distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01::m_vCrashedExit(
        int ai_iExitCode
    )	{
        Q_ASSERT(!m_qsApplication.isNull());
        Q_ASSERT(m_pqCommand);

        QJsonObject l_qjoProcessCrashed;
        l_qjoProcessCrashed.insert(
            uc_pcError,
            QObject::tr("Application: ") + m_qsApplication + QObject::tr("crashed")
        );
        l_qjoProcessCrashed.insert(uc_pcExitCode, ai_iExitCode);

        m_DistrAlgsThreadAddress.send(
            distrmsgs::distrmsgs_ProcessErrorEvent(
                m_qsApplication,
                l_qjoProcessCrashed,
                m_pqCommand
            )
        );
    }

    bool distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01::m_bWriteCommunicationFile(
        QString const& airc_qsJsonCommunicationFile,
        QByteArray const& airc_qbaLevelJsonObject
    )   {
        Q_ASSERT(!airc_qsJsonCommunicationFile.isEmpty());

        auto l_qslJsonCommunicationFileDirectory = airc_qsJsonCommunicationFile.split("/");
        Q_ASSERT(!l_qslJsonCommunicationFileDirectory.empty() );

        l_qslJsonCommunicationFileDirectory.pop_back();

        auto l_qsJsonCommunicationFileDirectory =  l_qslJsonCommunicationFileDirectory.join("/");

        QDir l_qdirJsonCommunicationFileDirectory(l_qsJsonCommunicationFileDirectory);  
        if (!l_qdirJsonCommunicationFileDirectory.exists()) {
            auto l_bPathCreated = l_qdirJsonCommunicationFileDirectory.mkpath(l_qdirJsonCommunicationFileDirectory.path());
            if (!l_bPathCreated)    {
                return false;
            }
        }

        QFile l_qfJsonCommunicationFile(airc_qsJsonCommunicationFile);
        if (!l_qfJsonCommunicationFile.open(QFile::WriteOnly))  {
            return false;
        }

        QTextStream l_qtsJsonCommunicationStream(&l_qfJsonCommunicationFile);
        l_qtsJsonCommunicationStream
            <<  QString(airc_qbaLevelJsonObject)
        ;

        if (!l_qfJsonCommunicationFile.flush()) {
            return false;
        }
        l_qfJsonCommunicationFile.close();

        return true;
    }

    void distributive_AutoStructCalcs::distributive_AutoStructCalcsImpl01::m_vSendProcessStartedLogMessage(
        QStringList const& airc_qslArguments
    )    {
        auto l_qjoProcessStartedLogMessage = QJsonObject();

        QJsonArray l_qjrArguments;
        for (auto l_qsArgument : airc_qslArguments)    {
            l_qjrArguments.push_back(l_qsArgument);
        }

        l_qjoProcessStartedLogMessage.insert(uc_pcArguments, l_qjrArguments);

        m_StdOutThreadAddress.send(
            io::stdoutmsgs::stdoutmsgs_LocalLogState()
        );
        qDebug(logDebug).noquote()
            <<  "the programs we are going to launch is: " + m_qsApplication
            <<  "the arguments are: " + airc_qslArguments.join(" ")
            <<  "the message to be logged is: " + QString(QJsonDocument(l_qjoProcessStartedLogMessage).toJson())
        ;
/*
        m_StdOutThreadAddress.send(
            io::stdoutmsgs::stdoutmsgs_LocalProcessStarted(
                m_qsApplication,
                l_qjoProcessStartedLogMessage
            )
        );
*/

    }

/////////////distributive_AutoStructCalcs/////////////

// constructors
distributive_AutoStructCalcs::distributive_AutoStructCalcs(
    QString const& airc_qsApplication,
    QStringList const& airc_qslArguments,
    QJsonObject const& airc_qjoProcessInput,
    QUndoCommand* ai_pqCommand,
    QObject* ai_pParent
)
	:	QObject(ai_pParent)
{
	Q_ASSERT(!m_spAutoStructCalcsImpl);
    Q_ASSERT(ai_pqCommand);

	m_spAutoStructCalcsImpl.reset(
		new distributive_AutoStructCalcsImpl01(
            this,
            airc_qsApplication,
            airc_qslArguments,
            airc_qjoProcessInput,
            ai_pqCommand
        )
	);

}

distributive_AutoStructCalcs::~distributive_AutoStructCalcs() = default;

// operations
void distributive_AutoStructCalcs::m_vRegister(
    std::tuple<
        threads::registry::registry_MFCommandLineSender::type,
        threads::registry::registry_MFAlgsSender::type,
        threads::registry::registry_MFDistrAlgsSender::type,
        threads::registry::registry_MFDbInstanceSender::type,
        threads::registry::registry_MFNetSender::type,
        threads::registry::registry_MFStdOutSender::type,
        threads::registry::registry_MFStdErrSender::type
    > const& airc_tupleThreadAddresses
)   {
	Q_ASSERT(m_spAutoStructCalcsImpl);

    m_spAutoStructCalcsImpl->m_vRegister(airc_tupleThreadAddresses);
}

void distributive_AutoStructCalcs::m_vCancel()    {
	Q_ASSERT(m_spAutoStructCalcsImpl);

    m_spAutoStructCalcsImpl->m_vCancel();
}

// delegates
void distributive_AutoStructCalcs::slot_vOnExecuteProgram()   {
	Q_ASSERT(m_spAutoStructCalcsImpl);

    m_spAutoStructCalcsImpl->m_vOnExecuteProgram();
}

void distributive_AutoStructCalcs::slot_vOnErrorOccured(QProcess::ProcessError ai_eProcessError)   {
	Q_ASSERT(m_spAutoStructCalcsImpl);

    m_spAutoStructCalcsImpl->m_vOnErrorOccured(ai_eProcessError);
}

void distributive_AutoStructCalcs::slot_vOnCancelProcess(QString const& airc_qsApplication, QUndoCommand* ai_pqCommand)   {
	Q_ASSERT(m_spAutoStructCalcsImpl);
    Q_ASSERT(ai_pqCommand);

    m_spAutoStructCalcsImpl->m_vOnCancelProcess(airc_qsApplication, ai_pqCommand);
}

void distributive_AutoStructCalcs::slot_vOnStarted()  {
	Q_ASSERT(m_spAutoStructCalcsImpl);

    m_spAutoStructCalcsImpl->m_vOnStarted();
}

void distributive_AutoStructCalcs::slot_vOnFinished(
    int ai_iExitCode,
    QProcess::ExitStatus ai_eExitStatus
)   {
	Q_ASSERT(m_spAutoStructCalcsImpl);

    m_spAutoStructCalcsImpl->m_vOnFinished(ai_iExitCode, ai_eExitStatus);
}

void distributive_AutoStructCalcs::slot_vOnReadyReadStandardError()   {
	Q_ASSERT(m_spAutoStructCalcsImpl);

    m_spAutoStructCalcsImpl->m_vOnReadyReadStandardError();
}

void distributive_AutoStructCalcs::slot_vOnReadyReadStandardOutput()  {
	Q_ASSERT(m_spAutoStructCalcsImpl);

    m_spAutoStructCalcsImpl->m_vOnReadyReadStandardOutput();
}

	}	// namespace distributive
}	// namespace algs
