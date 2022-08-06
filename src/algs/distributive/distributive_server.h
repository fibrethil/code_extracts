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

#ifndef INCLUDED_DISTRIBUTIVE_SERVER_H
#define INCLUDED_DISTRIBUTIVE_SERVER_H

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

#ifndef INCLUDED_STD_TUPLE_H
#include <tuple>
#define INCLUDED_STD_TUPLE_H
#endif

class QJsonObject;
class QUndoCommand;
class QString;

namespace algs	{
	namespace distributive	{

/*********************************************
*
* interface
*
**********************************************/

class ALGS_LIB distributive_Server
	:	public QObject
{
	Q_OBJECT

	// data
	private:
		class distributive_ServerImpl01;
		QScopedPointer<
			distributive_ServerImpl01
		> m_spServerImpl;

    // helper methods
    private:
        void m_vInstantiatePimpl();
        threads::registry::registry_MFDistrAlgsSender::type m_GetMailBox();

	// constructors
	public:
		distributive_Server(QObject* ai_pParent);
		distributive_Server(distributive_Server const&) = delete;
		distributive_Server(distributive_Server&&) = delete;
		virtual ~distributive_Server();

	// operators
	public:
		distributive_Server& operator = (distributive_Server const&) = delete;
		distributive_Server& operator = (distributive_Server&&) = delete;

    // accessors
    public:

    // operations
    public:
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

    // delegates
    public Q_SLOTS:
        void slot_vOnStarted();

    // events
    Q_SIGNALS:
        void signal_vDistrAlgsThreadFinished();
        void signal_vDistrAlgsThreadError(QJsonObject const& airc_qsDistrAlgsThreadError);
        void signal_vMailBoxCreated(threads::registry::registry_MFDistrAlgsSender::type);

        void signal_vProcessStarted(QString const& airc_qsApplication, QUndoCommand* ai_pqCommand);
        void signal_vProcessProgress(QString const& airc_qsApplication, double ai_dProgress, QUndoCommand* ai_pqCommand);
        void signal_vProcessDone(
            QString const& airc_qsApplication,
            QJsonObject const& airc_qjoProcessOutput,
            QUndoCommand* ai_pCommand
        );
        void signal_vProcessFinished(
            QString const& airc_qsApplication,
            QJsonObject const& airc_qjoProcessOutput,
            QUndoCommand* ai_pCommand
        );
        void signal_vProcessError(
            QString const& airc_qsApplication,
            QJsonObject const& airc_qjoProcessOutput,
            QUndoCommand* ai_pCommand
        );
        void signal_vProcessCanceled(
            QString const& airc_qsApplication,
            QUndoCommand* ai_pCommand
        );
};

	}	// namespace distributive
}	// namespace algs

#endif
