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

#ifndef INCLUDED_BEAMS_BEAM_H
#define INCLUDED_BEAMS_BEAM_H

#ifndef INCLUDED_BEAMS_SUPPORTROLE_H
#include "beams_supportrole.h"
#define INCLUDED_BEAMS_SUPPORTROLE_H
#endif

#ifndef INCLUDED_BEAMS_LOADS_H
#include "beams_loads.h"
#define INCLUDED_BEAMS_LOADS_H
#endif

#ifndef INCLUDED_DB_EXPORT_H
#include <db/export.h>
#define INCLUDED_DB_EXPORT_H
#endif

#ifndef INCLUDED_STRUCTURES_SPIMPL_H
#include <structures/include/spimpl.h>
#define INCLUDED_STRUCTURES_SPIMPL_H
#endif

#ifndef INCLUDED_QMETATYPE_H
#include <QMetaType>
#define INCLUDED_QMETATYPE_H
#endif

#ifndef INCLUDED_QSHAREDPOINTER_HPP
#include <QSharedPointer>
#define INCLUDED_QSHAREDPOINTER_HPP
#endif

#ifndef INCLUDED_STD_MEMORY_H
#include <memory>
#define INCLUDED_STD_MEMORY_H
#endif

#ifndef INCLUDED_STD_TUPLE_H
#include <tuple>
#define INCLUDED_STD_TUPLE_H
#endif

#ifndef INCLUDED_QUUID_H
#include <QUuid>
#define INCLUDED_QUUID_H
#endif

class QString;
class QJsonObject;
class QDataStream;

namespace db	{
	namespace beams	{

class beams_Dokos;

/*********************************************
*
* interface
*
**********************************************/

class DB_LIB beams_Beam	{
    // types
    public:
        struct Id  {
            using type = QUuid;            
        };
        struct Label    {
            using type = QString;
        };

        using PositionStart = QPointF;
        using PositionEnd = QPointF;
        struct Position {
            using type = std::tuple<PositionStart, PositionEnd>;
        };

        using B0 = double;
        using D0 = double;
        using BPlateContribution = double;
        struct CrossSection {
            using type = std::tuple<B0, D0>;
        };
        struct Dimensions   {
            using type = std::tuple<CrossSection::type, BPlateContribution>;
        };

        using Boundary = int;
        struct Boundaries    {
            using type = std::tuple<Boundary, Boundary>;
        };

        using Kind = int;
        using Grade = int;
        struct Material {
            using type = std::tuple<Kind, Grade>;
        };

        using LoadCoefficient = double;
        using Load = double;
        struct LoadG  {
            using type = std::tuple<LoadCoefficient, Load>;
        };
        struct LoadQ  {
            using type = std::tuple<LoadCoefficient, Load>;
        };
        struct LoadGep  {
            using type = std::tuple<LoadCoefficient, Load>;
        };

        struct SupportRole    {
            using type = beams_SupportRole;
        };

        struct Loads    {
            using type = beams_Loads;
        };

        struct LoadKind {
            using type = QString;
        };
        struct Weight   {
            double m_dWCoeff;
            double m_dW;
        };
        struct LoadCombinationComponent {
            using type = Weight;
        };

        struct Beam {
            using type = std::tuple<
                    Id::type,
                    Label::type,
                    Position::type,
                    Dimensions::type,
                    Boundaries::type,
                    Material::type,
                    SupportRole::type,
                    Loads::type
                >
            ;
        };

	// data
	private:
		class beams_BeamImpl01;
		spimpl::impl_ptr<
			beams_BeamImpl01
		> m_spBeamImpl;

	// constructors
	public:
        beams_Beam();
		beams_Beam(
            QString const& airc_qsStathmi
        );
		beams_Beam(
            QString const& airc_qsStathmi,
            Beam::type const&
        );
		beams_Beam(
            QString const& airc_qsStathmi,
            QJsonObject const& airc_qjoBeam
        );
		beams_Beam(
            QUuid const& airc_quuidId,
            QString const& airc_qsStathmi,
            beams_Dokos const& airc_Dokos
        );
		beams_Beam(beams_Beam const&) = default;
		beams_Beam(beams_Beam&&) = default;
		~beams_Beam() = default;


	// operators
	public:
		beams_Beam& operator = (beams_Beam const&) = default;
		beams_Beam& operator = (beams_Beam&&) = default;

    // accessors
    public:
        QUuid mc_quuidGetId()   const;
        QString mc_qsGetLabel() const;
        Position::type mc_tupleGetPosition()    const;
        Dimensions::type mc_tupleGetDimensions()    const;
        Beam::type mc_tupleGetBeam()   const;

    // modifiers
    public:
        void m_vSetId(QUuid const& airc_quuidId);
        void m_vSetLabel(QString const& airc_qsLabel);
        void m_vSetPosition(Position::type const& airc_tuplePosition);
        void m_vSetDimensions(Dimensions::type const& airc_tupleDimensions);
        void m_vSetBeam(Beam::type const&);

    // operations
    public:
        QJsonObject mc_qjoToJson()   const;
        void m_vFromJson(QJsonObject const&);

        beams_Dokos mc_ToDokos()  const;
        void m_vFromDokos(beams_Dokos const& airc_Dokos);
};

// streamers
QDataStream& operator << (
	QDataStream& ao_rDataStream,
	beams_Beam const& airc_Beam
);

QDataStream& operator >> (
	QDataStream& ao_rDataStream,
	beams_Beam& ao_rBeam
);

QDataStream& operator << (
	QDataStream& ao_rDataStream,
	std::shared_ptr<beams_Beam> const& airc_spBeam
);

QDataStream& operator >> (
	QDataStream& ao_rDataStream,
	std::shared_ptr<beams_Beam>& aor_spBeam
);


	}	// namespace beams
}	// namespace db

Q_DECLARE_METATYPE(db::beams::beams_Beam);
Q_DECLARE_METATYPE(db::beams::beams_Beam::LoadCombinationComponent::type);

#endif
