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

#ifndef INCLUDED_DISTRIBUTIVE_AUTOSTRUCTCALCSMANAGER_H
#define INCLUDED_DISTRIBUTIVE_AUTOSTRUCTCALCSMANAGER_H

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

#ifndef _NCLUDED_ALGS_EXPORT_H
#include <algs/export.h>
#define _NCLUDED_ALGS_EXPORT_H
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

class ALGS_LIB distributive_AutoStructCalcsManager
	:	public QObject
{
	Q_OBJECT

	// data
	private:
		class distributive_AutoStructCalcsManagerImpl01;
		QScopedPointer<
			distributive_AutoStructCalcsManagerImpl01
		> m_spAutoStructCalcsManagerImpl;


	// constructors
	public:
		distributive_AutoStructCalcsManager(QObject* ai_pParent);
		distributive_AutoStructCalcsManager(distributive_AutoStructCalcsManager const&) = delete;
		distributive_AutoStructCalcsManager(distributive_AutoStructCalcsManager&&) = delete;
		virtual ~distributive_AutoStructCalcsManager();

	// operators
	public:
		distributive_AutoStructCalcsManager& operator = (
            distributive_AutoStructCalcsManager const&
        ) = delete;
		distributive_AutoStructCalcsManager& operator = (
            distributive_AutoStructCalcsManager&&
        ) = delete;

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
            QJsonObject const& airc_qjoReport
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
    public Q_SLOTS:
        void slot_vOnLaunchAutoStructRc(
                QString const& airc_qsApplication,
                QStringList const& airc_qslArguments,
                QJsonObject const& airc_qjoProcessInput,
                QUndoCommand* ai_pqCommand
            )
        ;
	
    // events
    Q_SIGNALS:
        void signal_vExecuteProgram();
        void signal_vCancelProcess(
            QString const& airc_qsApplication,
            QUndoCommand* ai_pqCommand
        );
};

	}	// namespace distributive
}	// namespace algs

#endif
