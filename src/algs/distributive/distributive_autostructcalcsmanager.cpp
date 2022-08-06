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

#include "distributive_autostructcalcsmanager.h"

#include "distributive_autostructcalcs.h"
#include "distributive_autostructrc.h"
#include <io/include/stdoutput_locallogstate.h>
#include <io/include/stdoutmsgs_locallogstate.h>
#include <io/include/stdoutmsgs_localprocessdone.h>
#include <io/include/stdoutmsgs_localprocessstarted.h>
#include <io/include/stdoutmsgs_localprocessfinished.h>
#include <io/include/stdoutmsgs_localprocesserror.h>
#include <io/include/stdoutmsgs_localprocesscanceled.h>
#include <io/include/stderror_locallogstate.h>
#include <io/include/stderrmsgs_locallogstate.h>
#include <io/include/stderrmsgs_localprocesserror.h>
#include <tools/include/logger_categories.h>

#include <QtGlobal>
#include <QtDebug>
#include <QThread>
#include <QMap>
#include <QUndoCommand>
#include <QJsonObject>
#include <QJsonDocument>
#include <QStringList>
#include <QString>

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

	namespace algs		{
		namespace distributive		{

	/////////////helper classes//////////////////

	/*********************************************
	*
	* interface distributive_AutoStructCalcsManagerImpl01
	*
	**********************************************/

	class distributive_AutoStructCalcsManager::distributive_AutoStructCalcsManagerImpl01		{
        // types
        private:
            struct ProcessId    {
                using type = QString;
            };

		// data
		private:
			distributive_AutoStructCalcsManager* m_pAutoStructCalcsManager;

            bool m_bThreadAddressesRegistered   { false };
            QMap<ProcessId::type, QThread*> m_qmapThreads;
            QMap<ProcessId::type, QObject*> m_qmapAutoStructCalcsManager;

            threads::registry::registry_MFCommandLineSender::type m_PyThreadAddress;
            threads::registry::registry_MFAlgsSender::type m_AlgsThreadAddress;
            threads::registry::registry_MFAlgsSender::type m_DistrAlgsThreadAddress;
            threads::registry::registry_MFDbInstanceSender::type m_DbThreadAddress;
            threads::registry::registry_MFNetSender::type m_NetThreadAddress;
            threads::registry::registry_MFStdOutSender::type m_StdOutThreadAddress;
            threads::registry::registry_MFStdErrSender::type m_StdErrThreadAddress;
	
		// constructors
		public:
			distributive_AutoStructCalcsManagerImpl01() = default;
			distributive_AutoStructCalcsManagerImpl01(distributive_AutoStructCalcsManager*);
			distributive_AutoStructCalcsManagerImpl01(
                distributive_AutoStructCalcsManagerImpl01 const&
            ) = default;
			distributive_AutoStructCalcsManagerImpl01(
                distributive_AutoStructCalcsManagerImpl01&&
            ) = default;
			~distributive_AutoStructCalcsManagerImpl01();


		// operators
		public:
			distributive_AutoStructCalcsManagerImpl01& operator = (
                distributive_AutoStructCalcsManagerImpl01 const&
            ) = default;
			distributive_AutoStructCalcsManagerImpl01& operator = (
                distributive_AutoStructCalcsManagerImpl01&&
            ) = default;

        // queries
        public:
            bool mc_bThreadAddressesRegistered()  const;

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
            void m_vLaunchProcess(
                QString const& airc_qsApplication,
                QStringList const& airc_qslArguments,
                QJsonObject const& airc_qjoProcessInput,
                QUndoCommand* ai_pqCommand
            );
            void m_vProcessOutput(
                QJsonObject const& airc_qjoOutput
            );
            void m_vProcessDone(
                QString const& airc_qsApplication,
                QJsonObject const& airc_qjoProcessDone,
                QUndoCommand* ai_pqCommand
            );
            void m_vProcessStarted(
                QString const& airc_qsApplication,
                QJsonObject const& airc_qjoProcessStarted
            );
            void m_vProcessFinished(
                QString const& airc_qsApplication,
                QJsonObject const& airc_qjoProcessFinished,
                QUndoCommand* ai_pqCommand
            );
            void m_vProcessError(
                QString const& airc_qsApplication,
                QJsonObject const& airc_qjoProcessError,
                QUndoCommand* ai_pqCommand
            );
            void m_vProcessCanceled(
                QString const& airc_qsApplication,
                QJsonObject const& airc_qjoProcessCanceled,
                QUndoCommand* ai_pqCommand
            );

        // delegates
        public:
            void m_vOnLaunchAutoStructRc(
                QString const& airc_qsApplication,
                QStringList const& airc_qslArguments,
                QJsonObject const& airc_qjoProcessInput,
                QUndoCommand* ai_pqCommand
            );
	};

	/*********************************************
	*
	* implementation distributive_AutoStructCalcsManagerImpl01	
	*
	**********************************************/
	// constructors
	distributive_AutoStructCalcsManager::distributive_AutoStructCalcsManagerImpl01::
    distributive_AutoStructCalcsManagerImpl01(
        distributive_AutoStructCalcsManager* ai_pAutoStructCalcsManager
    )
		:	m_pAutoStructCalcsManager(ai_pAutoStructCalcsManager)
	{
		Q_ASSERT(m_pAutoStructCalcsManager);
	}

	distributive_AutoStructCalcsManager::distributive_AutoStructCalcsManagerImpl01::~distributive_AutoStructCalcsManagerImpl01()    {
        Q_ASSERT(m_qmapThreads.empty());
    }

    // delegates
    void
    distributive_AutoStructCalcsManager::distributive_AutoStructCalcsManagerImpl01::m_vOnLaunchAutoStructRc(
        QString const& airc_qsApplication,
        QStringList const& airc_qslArguments,
        QJsonObject const& airc_qjoProcessInput,
        QUndoCommand* ai_pqCommand
    )   {
        Q_ASSERT(m_pAutoStructCalcsManager);
        Q_ASSERT(!airc_qsApplication.isEmpty());
        Q_ASSERT(26 == airc_qslArguments.size());
        Q_ASSERT(ai_pqCommand);

        auto l_pqThread = new QThread(m_pAutoStructCalcsManager);
        auto l_pAutoStructRc =
            new distributive_AutoStructRc(
                airc_qsApplication,
                airc_qslArguments,
                airc_qjoProcessInput,
                ai_pqCommand,
                nullptr
            )
        ;
        Q_ASSERT(l_pAutoStructRc);

        Q_ASSERT(!m_PyThreadAddress.isNull());
        Q_ASSERT(!m_AlgsThreadAddress.isNull());
        Q_ASSERT(!m_DistrAlgsThreadAddress.isNull());
        Q_ASSERT(!m_DbThreadAddress.isNull());
        Q_ASSERT(!m_NetThreadAddress.isNull());
        Q_ASSERT(!m_StdOutThreadAddress.isNull());
        Q_ASSERT(!m_StdErrThreadAddress.isNull());

        l_pAutoStructRc->m_vRegister(
            std::make_tuple(
                m_PyThreadAddress,
                m_AlgsThreadAddress,
                m_DistrAlgsThreadAddress,
                m_DbThreadAddress,
                m_NetThreadAddress,
                m_StdOutThreadAddress,
                m_StdErrThreadAddress
            )
        );

        l_pAutoStructRc->moveToThread(l_pqThread);

        QObject::connect(
            m_pAutoStructCalcsManager,
            &distributive_AutoStructCalcsManager::signal_vExecuteProgram,
            l_pAutoStructRc,
            &distributive_AutoStructRc::slot_vOnExecuteProgram
        );

        QObject::connect(
            m_pAutoStructCalcsManager,
            &distributive_AutoStructCalcsManager::signal_vCancelProcess,
            l_pAutoStructRc,
            &distributive_AutoStructRc::slot_vOnCancelProcess
        );

        Q_ASSERT(!m_qmapThreads.contains(airc_qsApplication));
        m_qmapThreads[airc_qsApplication] = l_pqThread;

        Q_ASSERT(!m_qmapAutoStructCalcsManager.contains(airc_qsApplication));
        m_qmapAutoStructCalcsManager[airc_qsApplication] = l_pAutoStructRc;
        
        qDebug(logDebug).noquote()
            <<  "the programs we are going to launch is: " + airc_qsApplication
            <<  "the arguments are: " + airc_qslArguments.join(" ")
            <<  "the data input is: " + QString(QJsonDocument(airc_qjoProcessInput).toJson())
        ;

        l_pqThread->start();

        Q_EMIT m_pAutoStructCalcsManager->signal_vExecuteProgram();

        QObject::disconnect(
            m_pAutoStructCalcsManager,
            &distributive_AutoStructCalcsManager::signal_vExecuteProgram,
            l_pAutoStructRc,
            &distributive_AutoStructRc::slot_vOnExecuteProgram
        );
    }

    // operations
    void distributive_AutoStructCalcsManager::distributive_AutoStructCalcsManagerImpl01::m_vRegister(
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

        m_bThreadAddressesRegistered = true;
    }

    void distributive_AutoStructCalcsManager::distributive_AutoStructCalcsManagerImpl01::
    m_vLaunchProcess(
        QString const& airc_qsApplication,
        QStringList const& airc_qslArguments,
        QJsonObject const& airc_qjoProcessInput,
        QUndoCommand* ai_pqCommand
    )   {
        Q_ASSERT(m_pAutoStructCalcsManager);
        Q_ASSERT(!airc_qsApplication.isEmpty());
        Q_ASSERT(26 == airc_qslArguments.size());
        Q_ASSERT(ai_pqCommand);

        if (airc_qslArguments[11] == airc_qsApplication)    {
            m_pAutoStructCalcsManager->slot_vOnLaunchAutoStructRc(
                airc_qsApplication,
                airc_qslArguments,
                airc_qjoProcessInput,
                ai_pqCommand
            );

            return;
        }

        auto l_pqThread = new QThread(m_pAutoStructCalcsManager);
        auto l_pAutoStructCalcs =
            new distributive_AutoStructCalcs(
                airc_qsApplication,
                airc_qslArguments,
                airc_qjoProcessInput,
                ai_pqCommand,
                nullptr
            )
        ;
        Q_ASSERT(l_pAutoStructCalcs);

        Q_ASSERT(!m_PyThreadAddress.isNull());
        Q_ASSERT(!m_AlgsThreadAddress.isNull());
        Q_ASSERT(!m_DistrAlgsThreadAddress.isNull());
        Q_ASSERT(!m_DbThreadAddress.isNull());
        Q_ASSERT(!m_NetThreadAddress.isNull());
        Q_ASSERT(!m_StdOutThreadAddress.isNull());
        Q_ASSERT(!m_StdErrThreadAddress.isNull());

        l_pAutoStructCalcs->m_vRegister(
            std::make_tuple(
                m_PyThreadAddress,
                m_AlgsThreadAddress,
                m_DistrAlgsThreadAddress,
                m_DbThreadAddress,
                m_NetThreadAddress,
                m_StdOutThreadAddress,
                m_StdErrThreadAddress
            )
        );

        l_pAutoStructCalcs->moveToThread(l_pqThread);

        QObject::connect(
            m_pAutoStructCalcsManager,
            &distributive_AutoStructCalcsManager::signal_vExecuteProgram,
            l_pAutoStructCalcs,
            &distributive_AutoStructCalcs::slot_vOnExecuteProgram
        );

        QObject::connect(
            m_pAutoStructCalcsManager,
            &distributive_AutoStructCalcsManager::signal_vCancelProcess,
            l_pAutoStructCalcs,
            &distributive_AutoStructCalcs::slot_vOnCancelProcess
        );
/*
        QObject::connect(
            l_pAutoStructCalcs,
            &distributive_AutoStructCalcs::signal_vLaunchAutoStructRc,
            m_pAutoStructCalcsManager,
            &distributive_AutoStructCalcsManager::slot_vOnLaunchAutoStructRc
        );
*/
        Q_ASSERT(!m_qmapThreads.contains(airc_qsApplication));
        m_qmapThreads[airc_qsApplication] = l_pqThread;

        Q_ASSERT(!m_qmapAutoStructCalcsManager.contains(airc_qsApplication));
        m_qmapAutoStructCalcsManager[airc_qsApplication] = l_pAutoStructCalcs;
        
        qDebug(logDebug).noquote()
            <<  "the programs we are going to launch is: " + airc_qsApplication
            <<  "the arguments are: " + airc_qslArguments.join(" ")
            <<  "the data input is: " + QString(QJsonDocument(airc_qjoProcessInput).toJson())
        ;

        l_pqThread->start();

        Q_EMIT m_pAutoStructCalcsManager->signal_vExecuteProgram();

        QObject::disconnect(
            m_pAutoStructCalcsManager,
            &distributive_AutoStructCalcsManager::signal_vExecuteProgram,
            l_pAutoStructCalcs,
            &distributive_AutoStructCalcs::slot_vOnExecuteProgram
        );
    }

    void distributive_AutoStructCalcsManager::distributive_AutoStructCalcsManagerImpl01::
    m_vProcessOutput(
        QJsonObject const& airc_qjoOutput
    )   {
        Q_ASSERT(m_pAutoStructCalcsManager);

    }

    void distributive_AutoStructCalcsManager::distributive_AutoStructCalcsManagerImpl01::m_vProcessDone(
        QString const& airc_qsApplication,
        QJsonObject const& airc_qjoProcessDone,
        QUndoCommand* ai_pqCommand
    )   {
        Q_ASSERT(m_pAutoStructCalcsManager);
        Q_ASSERT(!airc_qsApplication.isEmpty());

        Q_ASSERT(m_qmapThreads.contains(airc_qsApplication));
        Q_ASSERT(m_qmapAutoStructCalcsManager.contains(airc_qsApplication));

        QObject* l_pAutoStructProcess = nullptr;
        l_pAutoStructProcess = qobject_cast<
                distributive_AutoStructCalcs*
            >(
                m_qmapAutoStructCalcsManager.value(airc_qsApplication)
            )
        ;
        if (!l_pAutoStructProcess)  {
            auto l_pAutoStructRc = qobject_cast<
                    distributive_AutoStructRc*
                >(
                    m_qmapAutoStructCalcsManager.value(airc_qsApplication)
                )
            ;
            Q_ASSERT(l_pAutoStructRc);

            QObject::disconnect(
                m_pAutoStructCalcsManager,
                &distributive_AutoStructCalcsManager::signal_vCancelProcess,
                l_pAutoStructRc,
                &distributive_AutoStructRc::slot_vOnCancelProcess
            );
        }
        else    {
            auto l_pAutoStructCalcs = qobject_cast<
                    distributive_AutoStructCalcs*
                >(
                    m_qmapAutoStructCalcsManager.value(airc_qsApplication)
                )
            ;
            Q_ASSERT(l_pAutoStructCalcs);

            QObject::disconnect(
                m_pAutoStructCalcsManager,
                &distributive_AutoStructCalcsManager::signal_vCancelProcess,
                l_pAutoStructCalcs,
                &distributive_AutoStructCalcs::slot_vOnCancelProcess
            );
        }

        m_StdOutThreadAddress.send(
            io::stdoutmsgs::stdoutmsgs_LocalLogState()
        );
        m_StdOutThreadAddress.send(
            io::stdoutmsgs::stdoutmsgs_LocalProcessDone(
                airc_qsApplication,
                airc_qjoProcessDone
            )
        );
    }

    void distributive_AutoStructCalcsManager::distributive_AutoStructCalcsManagerImpl01::m_vProcessStarted(
        QString const& airc_qsApplication,
        QJsonObject const& airc_qjoProcessStarted
    )   {
        Q_ASSERT(m_pAutoStructCalcsManager);
        Q_ASSERT(!airc_qsApplication.isEmpty());

        m_StdOutThreadAddress.send(
            io::stdoutmsgs::stdoutmsgs_LocalLogState()
        );
        m_StdOutThreadAddress.send(
            io::stdoutmsgs::stdoutmsgs_LocalProcessStarted(
                airc_qsApplication,
                airc_qjoProcessStarted
            )
        );
    }

    void distributive_AutoStructCalcsManager::distributive_AutoStructCalcsManagerImpl01::m_vProcessFinished(
        QString const& airc_qsApplication,
        QJsonObject const& airc_qjoProcessFinished,
        QUndoCommand* ai_pqCommand
    )   {
        Q_ASSERT(m_pAutoStructCalcsManager);
        Q_ASSERT(!airc_qsApplication.isEmpty());

        Q_ASSERT(m_qmapThreads.contains(airc_qsApplication));
        Q_ASSERT(m_qmapAutoStructCalcsManager.contains(airc_qsApplication));

        m_StdOutThreadAddress.send(
            io::stdoutmsgs::stdoutmsgs_LocalLogState()
        );
        m_StdOutThreadAddress.send(
            io::stdoutmsgs::stdoutmsgs_LocalProcessFinished(
                airc_qsApplication,
                airc_qjoProcessFinished
            )
        );

        auto l_pqFinishedProcessThread = m_qmapThreads.value(airc_qsApplication);
        Q_ASSERT(l_pqFinishedProcessThread);

        auto l_pAutoStructLaunchedProcess = m_qmapAutoStructCalcsManager.value(airc_qsApplication);
        Q_ASSERT(l_pAutoStructLaunchedProcess);

        m_qmapThreads.remove(airc_qsApplication);
        m_qmapAutoStructCalcsManager.remove(airc_qsApplication);

        qDebug(logDebug).noquote()
            <<  "we remove the thread of the program: " + airc_qsApplication
        ;

        l_pqFinishedProcessThread->quit();
        l_pqFinishedProcessThread->wait();

        l_pAutoStructLaunchedProcess->deleteLater();
        l_pqFinishedProcessThread->deleteLater();
    }

    void distributive_AutoStructCalcsManager::distributive_AutoStructCalcsManagerImpl01::m_vProcessError(
        QString const& airc_qsApplication,
        QJsonObject const& airc_qjoProcessError,
        QUndoCommand* ai_pqCommand
    )   {
        Q_ASSERT(m_pAutoStructCalcsManager);
        Q_ASSERT(!airc_qsApplication.isEmpty());

        Q_ASSERT(m_qmapThreads.contains(airc_qsApplication));
        Q_ASSERT(m_qmapAutoStructCalcsManager.contains(airc_qsApplication));

        m_StdOutThreadAddress.send(
            io::stdoutmsgs::stdoutmsgs_LocalLogState()
        );
        m_StdOutThreadAddress.send(
            io::stdoutmsgs::stdoutmsgs_LocalProcessError(
                airc_qsApplication,
                airc_qjoProcessError
            )
        );

        m_StdErrThreadAddress.send(
            io::stderrmsgs::stderrmsgs_LocalLogState()
        );
        m_StdErrThreadAddress.send(
            io::stderrmsgs::stderrmsgs_LocalProcessError(
                airc_qsApplication,
                airc_qjoProcessError
            )
        );

        auto l_pqErrorProcessThread = m_qmapThreads.value(airc_qsApplication);
        Q_ASSERT(l_pqErrorProcessThread);

        auto l_pAutoStructLaunchedProcess = m_qmapAutoStructCalcsManager.value(airc_qsApplication);
        Q_ASSERT(l_pAutoStructLaunchedProcess);

        m_qmapThreads.remove(airc_qsApplication);
        m_qmapAutoStructCalcsManager.remove(airc_qsApplication);

        qDebug(logDebug).noquote()
            <<  "we remove the thread of the program: " + airc_qsApplication
        ;

        l_pqErrorProcessThread->quit();
        l_pqErrorProcessThread->wait();

        l_pAutoStructLaunchedProcess->deleteLater();
        l_pqErrorProcessThread->deleteLater();
    }

    void distributive_AutoStructCalcsManager::distributive_AutoStructCalcsManagerImpl01::m_vProcessCanceled(
        QString const& airc_qsApplication,
        QJsonObject const& airc_qjoProcessCanceled,
        QUndoCommand* ai_pqCommand
    )   {
        Q_ASSERT(m_pAutoStructCalcsManager);
        Q_ASSERT(!airc_qsApplication.isEmpty());

        Q_ASSERT(m_qmapThreads.contains(airc_qsApplication));
        Q_ASSERT(m_qmapAutoStructCalcsManager.contains(airc_qsApplication));

        m_StdOutThreadAddress.send(
            io::stdoutmsgs::stdoutmsgs_LocalLogState()
        );
        m_StdOutThreadAddress.send(
            io::stdoutmsgs::stdoutmsgs_LocalProcessCanceled(
                airc_qsApplication,
                airc_qjoProcessCanceled
            )
        );

        auto l_pqCanceledProcessThread = m_qmapThreads.value(airc_qsApplication);
        Q_ASSERT(l_pqCanceledProcessThread);

        auto l_pAutoStructLaunchedProcess = m_qmapAutoStructCalcsManager.value(airc_qsApplication);
        Q_ASSERT(l_pAutoStructLaunchedProcess);

        m_qmapThreads.remove(airc_qsApplication);
        m_qmapAutoStructCalcsManager.remove(airc_qsApplication);

        qDebug(logDebug).noquote()
            <<  "we remove the thread of the program: " + airc_qsApplication
        ;

        l_pqCanceledProcessThread->quit();
        l_pqCanceledProcessThread->wait();

        l_pAutoStructLaunchedProcess->deleteLater();
        l_pqCanceledProcessThread->deleteLater();
    }

    // queries
    bool distributive_AutoStructCalcsManager::distributive_AutoStructCalcsManagerImpl01::
    mc_bThreadAddressesRegistered()  const  {
        return m_bThreadAddressesRegistered;
    }

/////////////distributive_AutoStructCalcsManager/////////////

// constructors
distributive_AutoStructCalcsManager::distributive_AutoStructCalcsManager(QObject* ai_pParent)
	:	QObject(ai_pParent)
{
	Q_ASSERT(!m_spAutoStructCalcsManagerImpl);

	m_spAutoStructCalcsManagerImpl.reset(
		new distributive_AutoStructCalcsManagerImpl01(this)
	);

}

distributive_AutoStructCalcsManager::~distributive_AutoStructCalcsManager() = default;

// operations
void distributive_AutoStructCalcsManager::m_vRegister(
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
	Q_ASSERT(m_spAutoStructCalcsManagerImpl);

    m_spAutoStructCalcsManagerImpl->m_vRegister(airc_tupleThreadAddresses);
}
void distributive_AutoStructCalcsManager::m_vLaunchProcess(
    QString const& airc_qsApplication,
    QStringList const& airc_qslArguments,
    QJsonObject const& airc_qjoProcessInput,
    QUndoCommand* ai_pqCommand
)   {
	Q_ASSERT(m_spAutoStructCalcsManagerImpl);
    Q_ASSERT(!airc_qsApplication.isEmpty());

    m_spAutoStructCalcsManagerImpl->m_vLaunchProcess(airc_qsApplication, airc_qslArguments, airc_qjoProcessInput, ai_pqCommand);
}

void distributive_AutoStructCalcsManager::m_vProcessOutput(
    QJsonObject const& airc_qjoOutput
)   {
	Q_ASSERT(m_spAutoStructCalcsManagerImpl);

    m_spAutoStructCalcsManagerImpl->m_vProcessOutput(airc_qjoOutput);
}

void distributive_AutoStructCalcsManager::m_vProcessDone(
    QString const& airc_qsApplication,
    QJsonObject const& airc_qjoProcessDone,
    QUndoCommand* ai_pqCommand
)   {
	Q_ASSERT(m_spAutoStructCalcsManagerImpl);
    Q_ASSERT(!airc_qsApplication.isEmpty());
    Q_ASSERT(ai_pqCommand);

    m_spAutoStructCalcsManagerImpl->m_vProcessDone(
        airc_qsApplication,
        airc_qjoProcessDone,
        ai_pqCommand
    );
}

void distributive_AutoStructCalcsManager::m_vProcessStarted(
    QString const& airc_qsApplication,
    QJsonObject const& airc_qjoProcessStarted
)   {
	Q_ASSERT(m_spAutoStructCalcsManagerImpl);
    Q_ASSERT(!airc_qsApplication.isEmpty());

    m_spAutoStructCalcsManagerImpl->m_vProcessStarted(
        airc_qsApplication,
        airc_qjoProcessStarted
    );
}

void distributive_AutoStructCalcsManager::m_vProcessFinished(
    QString const& airc_qsApplication,
    QJsonObject const& airc_qjoProcessFinished,
    QUndoCommand* ai_pqCommand
)   {
	Q_ASSERT(m_spAutoStructCalcsManagerImpl);
    Q_ASSERT(!airc_qsApplication.isEmpty());
    Q_ASSERT(ai_pqCommand);

    m_spAutoStructCalcsManagerImpl->m_vProcessFinished(
        airc_qsApplication,
        airc_qjoProcessFinished,
        ai_pqCommand
    );
}

void distributive_AutoStructCalcsManager::m_vProcessError(
    QString const& airc_qsApplication,
    QJsonObject const& airc_qjoProcessError,
    QUndoCommand* ai_pqCommand
)   {
	Q_ASSERT(m_spAutoStructCalcsManagerImpl);
    Q_ASSERT(!airc_qsApplication.isEmpty());
    Q_ASSERT(ai_pqCommand);

    m_spAutoStructCalcsManagerImpl->m_vProcessError(
        airc_qsApplication,
        airc_qjoProcessError,
        ai_pqCommand
    );
}

void distributive_AutoStructCalcsManager::m_vProcessCanceled(
    QString const& airc_qsApplication,
    QJsonObject const& airc_qjoProcessCanceled,
    QUndoCommand* ai_pqCommand
)   {
	Q_ASSERT(m_spAutoStructCalcsManagerImpl);
    Q_ASSERT(airc_qsApplication.isNull());
    Q_ASSERT(ai_pqCommand);

    m_spAutoStructCalcsManagerImpl->m_vProcessCanceled(
        airc_qsApplication,
       airc_qjoProcessCanceled,
        ai_pqCommand
    );
}

// queries
bool distributive_AutoStructCalcsManager::mc_bThreadAddressesRegistered()  const {
	Q_ASSERT(m_spAutoStructCalcsManagerImpl);

    return m_spAutoStructCalcsManagerImpl->mc_bThreadAddressesRegistered();
}

// private delegates
void distributive_AutoStructCalcsManager::slot_vOnLaunchAutoStructRc(
    QString const& airc_qsApplication,
    QStringList const& airc_qslArguments,
    QJsonObject const& airc_qjoProcessInput,
    QUndoCommand* ai_pqCommand
)   {
	Q_ASSERT(m_spAutoStructCalcsManagerImpl);

    m_spAutoStructCalcsManagerImpl->m_vOnLaunchAutoStructRc(
            airc_qsApplication,
            airc_qslArguments,
            airc_qjoProcessInput,
            ai_pqCommand
        )
    ;
}

	}	// namespace distributive
}	// namespace algs
