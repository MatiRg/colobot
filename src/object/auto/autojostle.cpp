// * This file is part of the COLOBOT source code
// * Copyright (C) 2001-2008, Daniel ROUX & EPSITEC SA, www.epsitec.ch
// *
// * This program is free software: you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation, either version 3 of the License, or
// * (at your option) any later version.
// *
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// * GNU General Public License for more details.
// *
// * You should have received a copy of the GNU General Public License
// * along with this program. If not, see  http://www.gnu.org/licenses/.

#define STRICT
#define D3D_OVERLOADS

#include <windows.h>
#include <stdio.h>
#include <d3d.h>

#include "math/const.h"
#include "common/struct.h"
#include "graphics/d3d/d3dengine.h"
#include "math/old/d3dmath.h"
#include "common/event.h"
#include "common/misc.h"
#include "common/iman.h"
#include "math/old/math3d.h"
#include "graphics/common/particule.h"
#include "graphics/common/light.h"
#include "graphics/common/terrain.h"
#include "graphics/common/camera.h"
#include "object/object.h"
#include "ui/interface.h"
#include "ui/button.h"
#include "ui/list.h"
#include "ui/window.h"
#include "sound/sound.h"
#include "object/auto/auto.h"
#include "object/auto/autojostle.h"




// Object's constructor.

CAutoJostle::CAutoJostle(CInstanceManager* iMan, CObject* object)
						   : CAuto(iMan, object)
{
	Init();
}

// Object's destructor.

CAutoJostle::~CAutoJostle()
{
	this->CAuto::~CAuto();
}


// Destroys the object.

void CAutoJostle::DeleteObject(BOOL bAll)
{
	CAuto::DeleteObject(bAll);
}


// Initialize the object.

void CAutoJostle::Init()
{
	m_time = 0.0f;
	m_error = ERR_CONTINUE;

	CAuto::Init();
}


// Start an emission.

void CAutoJostle::Start(int param, float force)
{
	ObjectType	type;

	if ( force < 0.0f )  force = 0.0f;
	if ( force > 1.0f )  force = 1.0f;

	m_force = force;
	m_progress = 0.0f;
	m_speed = 1.0f/(0.5f+force*1.0f);  // 0.5 .. 1.5
	m_time = 0.0f;
	m_error = ERR_CONTINUE;

	type = m_object->RetType();
	if ( type >= OBJECT_PLANT5 &&
		 type <= OBJECT_PLANT7 )  // clover?
	{
		m_force *= 3.0f;
	}
}


// Management of an event.

BOOL CAutoJostle::EventProcess(const Event &event)
{
	D3DVECTOR	dir;
	float		factor, angle, zoom;

	CAuto::EventProcess(event);

	if ( m_engine->RetPause() )  return TRUE;
	if ( event.event != EVENT_FRAME )  return TRUE;

	if ( m_progress < 1.0f )
	{
		m_progress += event.rTime*m_speed;

		if ( m_progress < 0.5f )
		{
			factor = m_progress/0.5f;
		}
		else
		{
			factor = 2.0f-m_progress/0.5f;
		}
		factor *= m_force;

		dir.x = sinf(m_progress*PI*4.0f);
		dir.z = cosf(m_progress*PI*4.0f);

		angle = sinf(m_time*10.0f)*factor*0.04f;
		m_object->SetAngleX(0, angle*dir.z);
		m_object->SetAngleZ(0, angle*dir.x);

		zoom = 1.0f+sinf(m_time*8.0f)*factor*0.06f;
		m_object->SetZoomX(0, zoom);
		zoom = 1.0f+sinf(m_time*5.0f)*factor*0.06f;
		m_object->SetZoomY(0, zoom);
		zoom = 1.0f+sinf(m_time*7.0f)*factor*0.06f;
		m_object->SetZoomZ(0, zoom);
	}
	else
	{
		m_object->SetAngleX(0, 0.0f);
		m_object->SetAngleZ(0, 0.0f);
		m_object->SetZoom(0, D3DVECTOR(1.0f, 1.0f, 1.0f));
		m_error = ERR_STOP;
	}

	return TRUE;
}


// Indicates whether the controller has completed its activity.

Error CAutoJostle::IsEnded()
{
	return m_error;
}

