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

#ifndef INCLUDED_DBPROXY_BEAM_H
#define INCLUDED_DBPROXY_BEAM_H

#ifndef INCLUDED_MODELS_EXPORT_H
#include <models/export.h>
#define INCLUDED_MODELS_EXPORT_H
#endif

#ifndef INCLUDED_BEAMS_BEAM_H
#include <db/include/beams_beam.h>
#define INCLUDED_BEAMS_BEAM_H
#endif

#ifndef INCLUDED_QSHAREDPOINTER_HPP
#include <QSharedPointer>
#define INCLUDED_QSHAREDPOINTER_HPP
#endif

#ifndef INCLUDED_QUUID_H
#include <QUuid>
#define INCLUDED_QUUID_H
#endif

#ifndef INCLUDED_STD_TUPLE_H
#include <tuple>
#define INCLUDED_STD_TUPLE_H
#endif

#ifndef INCLUDED_STRUCTURES_SPIMPL_H
#include <structures/include/spimpl.h>
#define INCLUDED_STRUCTURES_SPIMPL_H
#endif

#ifndef INCLUDED_STD_MEMORY_H
#include <memory>
#define INCLUDED_STD_MEMORY_H
#endif


namespace db    {
    namespace beams {
        class beams_Dokos;
    }   // namespace beams
    namespace vars  {
        class vars_TInstance;
    }   // namespace vars
}   // namespace db

class QString;
class QJsonObject;
class QDataStream;


namespace models	{
	namespace dbproxy	{

/*********************************************
*
* interface
*
**********************************************/

class MODELS_LIB dbproxy_Beam	{
    // types
    public:

	// data
	private:
		class dbproxy_BeamImpl01;
		spimpl::impl_ptr<
			dbproxy_BeamImpl01
		> m_spBeamImpl;

	// constructors
	public:
		dbproxy_Beam(
            QString const& airc_qsStathmi,
            QSharedPointer<db::vars::vars_TInstance> ai_qspInstance
        );
		dbproxy_Beam(
            QString const& airc_qsStathmi,
            QSharedPointer<db::vars::vars_TInstance> ai_qspInstance,
            db::beams::beams_Beam::Beam::type const&
        );
		dbproxy_Beam(
            QString const& airc_qsStathmi,
            QSharedPointer<db::vars::vars_TInstance> ai_qspInstance,
            QJsonObject const& airc_qjoBeam
        );
		dbproxy_Beam(dbproxy_Beam const&) = default;
		dbproxy_Beam(dbproxy_Beam&&) = default;
		~dbproxy_Beam() = default;


	// operators
	public:
		dbproxy_Beam& operator = (dbproxy_Beam const&) = default;
		dbproxy_Beam& operator = (dbproxy_Beam&&) = default;

    // accessors
    public:
        QUuid mc_quuidGetId()   const;
        QString mc_qsGetLabel() const;
        db::beams::beams_Beam::Position::type mc_tupleGetPosition()    const;
        db::beams::beams_Beam::Dimensions::type mc_tupleGetDimensions()    const;
        db::beams::beams_Beam::Beam::type mc_tupleGetBeam()   const;

    // modifiers
    public:
        void m_vSetId(QUuid const& airc_quuidId);
        void m_vSetLabel(QString const& airc_qsLabel);
        void m_vSetPosition(db::beams::beams_Beam::Position::type const& airc_tuplePosition);
        void m_vSetDimensions(db::beams::beams_Beam::Dimensions::type const& airc_tupleDimensions);
        void m_vSetBeam(db::beams::beams_Beam::Beam::type const&);

    // operations
    public:
        QJsonObject mc_qjoToJson()   const;
        void m_vFromJson(QJsonObject const&);

        db::beams::beams_Dokos mc_ToDokos()   const;
        void m_vFromDokos(db::beams::beams_Dokos const&);

    // friend streamers
    public:
        friend
        QDataStream& operator << (
        	QDataStream& ao_rDataStream,
        	dbproxy_Beam const& airc_DbProxyBeam
        );
        
        friend
        QDataStream& operator >> (
        	QDataStream& ao_rDataStream,
        	dbproxy_Beam& ao_rDbProxyBeam
        );
        
        friend
        QDataStream& operator << (
        	QDataStream& ao_rDataStream,
        	std::shared_ptr<dbproxy_Beam> const& airc_spDbProxyBeam
        );
        
        friend
        QDataStream& operator >> (
        	QDataStream& ao_rDataStream,
        	std::shared_ptr<dbproxy_Beam>& aor_spDbProxyBeam
        );
};

// streamers
QDataStream& operator << (
	QDataStream& ao_rDataStream,
	dbproxy_Beam const& airc_DbProxyBeam
);

QDataStream& operator >> (
	QDataStream& ao_rDataStream,
	dbproxy_Beam& ao_rDbProxyBeam
);

QDataStream& operator << (
	QDataStream& ao_rDataStream,
	std::shared_ptr<dbproxy_Beam> const& airc_spDbProxyBeam
);

QDataStream& operator >> (
	QDataStream& ao_rDataStream,
	std::shared_ptr<dbproxy_Beam>& aor_spDbProxyBeam
);


	}	// namespace dbproxy
}	// namespace models


#endif
