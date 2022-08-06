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

#ifndef INCLUDED_DISTRIBUTIVE_AUTOSTRUCTCALCS_H
#define INCLUDED_DISTRIBUTIVE_AUTOSTRUCTCALCS_H

#ifndef _NCLUDED_ALGS_EXPORT_H
#include <algs/export.h>
#define _NCLUDED_ALGS_EXPORT_H
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

#ifndef INCLUDED_QPROCESS_H
#include <QProcess>
#define INCLUDED_QPROCESS_H
#endif

#ifndef INCLUDED_QSCOPEDPOINTER_HPP
#include <QScopedPointer>
#define INCLUDED_QSCOPEDPOINTER_HPP
#endif

#ifndef INCLUDED_QOBJECT_H
#include <QObject>
#define INCLUDED_QOBJECT_H
#endif

class QUndoCommand;
class QJsonObject;
class QStringList;
class QString;

namespace algs	{
	namespace distributive	{

/*********************************************
*
* interface
*
**********************************************/

class ALGS_LIB distributive_AutoStructCalcs
	:	public QObject
{
	Q_OBJECT

	// data
	private:
		class distributive_AutoStructCalcsImpl01;
		QScopedPointer<
			distributive_AutoStructCalcsImpl01
		> m_spAutoStructCalcsImpl;

	// constructors
	public:
		distributive_AutoStructCalcs(
            QString const& airc_qsApplication,
            QStringList const& airc_qslArguments,
            QJsonObject const& airc_qjoProcessInput,
            QUndoCommand* ai_pqCommand,
            QObject* ai_pParent
        );
		distributive_AutoStructCalcs(distributive_AutoStructCalcs const&) = delete;
		distributive_AutoStructCalcs(distributive_AutoStructCalcs&&) = delete;
		virtual ~distributive_AutoStructCalcs();

	// operators
	public:
		distributive_AutoStructCalcs& operator = (distributive_AutoStructCalcs const&) = delete;
		distributive_AutoStructCalcs& operator = (distributive_AutoStructCalcs&&) = delete;

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
    public Q_SLOTS:
        void slot_vOnExecuteProgram();
        void slot_vOnErrorOccured(QProcess::ProcessError);
        void slot_vOnCancelProcess(QString const& airc_qsApplication, QUndoCommand* ai_pqCommand);
        void slot_vOnStarted();
        void slot_vOnFinished(int ai_iExitCode, QProcess::ExitStatus);
        void slot_vOnReadyReadStandardOutput();
        void slot_vOnReadyReadStandardError();

    // events
    Q_SIGNALS:
        void signal_vProcessFailedToStart();
        void signal_vProcessCrashed();
        void signal_vProcessTimeout();
        void signal_vProcessWriteError();
        void signal_vProcessReadError();
        void signal_vProcessUnknownError();
        void signal_vNormalExit(int ai_iExitCode);
        void signal_vCrashedExit(int ai_iExitCode);
        void signal_vReadyReadStandardError();
        void signal_vReadyReadStandardOutput();
        void signal_vLaunchAutoStructRc(
                QString const& airc_qsApplication,
                QStringList const& airc_qslArguments,
                QJsonObject const& airc_qjoProcessInput,
                QUndoCommand* ai_pqCommand
            )
        ;
	
};

	}	// namespace distributive
}	// namespace algs

#endif
