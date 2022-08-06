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

#ifndef INCLUDED_AUTOSTRUCTMANAGER_STATEMACHINE_H
#define INCLUDED_AUTOSTRUCTMANAGER_STATEMACHINE_H

#ifndef INCLUDED_STATES_EXPORT_H
#include <states/export.h>
#define INCLUDED_STATES_EXPORT_H
#endif

#ifndef INCLUDED_REGISTRY_MFCOMMANDLINESENDER_H
#include <threads/include/registry_mfcommandlinesender.hpp>
#define INCLUDED_REGISTRY_MFCOMMANDLINESENDER_H
#endif

#ifndef INCLUDED_REGISTRY_MFALGSSENDER_H
#include <threads/include/registry_mfalgssender.hpp>
#define INCLUDED_REGISTRY_MFALGSSENDER_H
#endif

#ifndef INCLUDED_REGISTRY_MFDISTRALGSSENDER_H
#include <threads/include/registry_mfdistralgssender.hpp>
#define INCLUDED_REGISTRY_MFDISTRALGSSSENDER_H
#endif

#ifndef INCLUDED_REGISTRY_MFDBINSTANCESENDER_H
#include <threads/include/registry_mfdbinstancesender.hpp>
#define INCLUDED_REGISTRY_MFDBINSTANCESENDER_H
#endif

#ifndef INCLUDED_REGISTRY_MFNETSENDER_H
#include <threads/include/registry_mfnetsender.hpp>
#define INCLUDED_REGISTRY_MFNETSENDER_H
#endif

#ifndef INCLUDED_REGISTRY_MFSTDOUTSENDER_H
#include <threads/include/registry_mfstdoutsender.hpp>
#define INCLUDED_REGISTRY_MFSTDOUTSENDER_H
#endif

#ifndef INCLUDED_REGISTRY_MFSTDERRSENDER_H
#include <threads/include/registry_mfstderrsender.hpp>
#define INCLUDED_REGISTRY_MFSTDERRSENDER_H
#endif

#ifndef INCLUDED_QSTATEMACHINE_H
#include <QStateMachine>
#define INCLUDED_QSTATEMACHINE_H
#endif

#ifndef INCLUDED_QOBJECT_H
#include <QObject>
#define INCLUDED_QOBJECT_H
#endif

#ifndef INCLUDED_QSCOPEDPOINTER_HPP
#include <QScopedPointer>
#define INCLUDED_QSCOPEDPOINTER_HPP
#endif

class QJsonObject;


namespace states	{
	namespace autostructmanager	{

/*********************************************
*
* interface
*
**********************************************/

class STATES_LIB autostructmanager_StateMachine
	:	public QStateMachine
{
	Q_OBJECT

	// data
	private:
		class autostructmanager_StateMachineImpl01;
		QScopedPointer<autostructmanager_StateMachineImpl01> m_spStateMachineImpl;

    // private delegates
    private Q_SLOTS:
        void slot_vOnThreadStarted();
        void slot_vOnThreadFinished();

        void slot_vOnPyThreadFinished();
        void slot_vOnPyThreadError(QJsonObject const& airc_qjoPyThreadError);

        void slot_vOnAlgsThreadFinished();
        void slot_vOnAlgsThreadError(QJsonObject const& airc_qjoPyThreadError);

        void slot_vOnDistrAlgsThreadFinished();
        void slot_vOnDistrAlgsThreadError(QJsonObject const& airc_qjoPyThreadError);

        void slot_vOnDbInstanceThreadFinished();
        void slot_vOnDbInstanceThreadError(QJsonObject const& airc_qjoPyThreadError);

        void slot_vOnNetThreadFinished();
        void slot_vOnNetThreadError(QJsonObject const& airc_qjoPyThreadError);

        void slot_vOnStdOutThreadFinished();
        void slot_vOnStdOutThreadError(QJsonObject const& airc_qjoPyThreadError);

        void slot_vOnStdErrThreadFinished();
        void slot_vOnStdErrThreadError(QJsonObject const& airc_qjoPyThreadError);

        void slot_vOnPyThreadMailBoxCreated(threads::registry::registry_MFCommandLineSender::type);
        void slot_vOnAlgsThreadMailBoxCreated(threads::registry::registry_MFAlgsSender::type);
        void slot_vOnDistrAlgsThreadMailBoxCreated(threads::registry::registry_MFDistrAlgsSender::type);
        void slot_vOnDbThreadMailBoxCreated(threads::registry::registry_MFDbInstanceSender::type);
        void slot_vOnNetThreadMailBoxCreated(threads::registry::registry_MFNetSender::type m_NetThreadAddress);
        void slot_vOnStdOutThreadMailBoxCreated(threads::registry::registry_MFStdOutSender::type);
        void slot_vOnStdErrThreadMailBoxCreated(threads::registry::registry_MFStdErrSender::type);

    // overriding functionalities
    private:
        virtual void onEntry(QEvent* ai_pEvent);
        virtual void onExit(QEvent* ai_pEvent);

	// constructors
	public:
		autostructmanager_StateMachine(
            QObject* ai_pParent
        );
		autostructmanager_StateMachine(autostructmanager_StateMachine const&) = delete;
		autostructmanager_StateMachine(autostructmanager_StateMachine&&) = delete;
		virtual ~autostructmanager_StateMachine();

	// operators
	public:
		autostructmanager_StateMachine& operator = (autostructmanager_StateMachine const&) = delete;
		autostructmanager_StateMachine& operator = (autostructmanager_StateMachine&&) = delete;

    // operations
    public:

    // operation slots
    public Q_SLOTS:
        void slot_vStart();
        void slot_vOnCloseThreads();
/*
    // events
    Q_SIGNALS:
        void signal_vRunPy();
        void signal_vRunAlgs();
        void signal_vRunDistrAlgs();
        void signal_vRunDb();
        void signal_vRunNet();
        void signal_vRunStdOut();
        void signal_vRunStdErr();
*/	
};

	}	// namespace autostructmanager
}	// namespace states

#endif
