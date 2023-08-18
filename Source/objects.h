/*
	Copyright (c) 2011, T. Kroes <t.kroes@tudelft.nl>
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	- Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "object.h"

namespace ExposureRender
{

HOST_DEVICE_NI void IntersectObject(const Object& Object, const Ray& R, ScatterEvent& RS)
{
	Ray Rt = TransformRay(Object.Shape.InvTM, R);

	Intersection Int;

	IntersectShape(Object.Shape, Rt, Int);

	if (Int.Valid)
	{
		RS.Valid	= true;
		RS.N 		= TransformVector(Object.Shape.TM, Int.N);
		RS.P 		= TransformPoint(Object.Shape.TM, Int.P);
		RS.T 		= Length(RS.P - R.O);
		RS.Wo		= -R.D;
		RS.Le		= ColorXYZf(0.0f);
		RS.UV		= Int.UV;
	}
}

HOST_DEVICE_NI void IntersectObjects(const Ray& R, ScatterEvent& RS)
{
	float T = FLT_MAX;

	for (int i = 0; i < gpTracer->ObjectIDs.Count; i++)
	{
		const Object& Object = gpObjects[i];

		ScatterEvent LocalRS(Enums::Object);

		LocalRS.ObjectID = i;

		IntersectObject(Object, R, LocalRS);

		if (LocalRS.Valid && LocalRS.T < T)
		{
			RS = LocalRS;
			T = LocalRS.T;
		}
	}
}

HOST_DEVICE_NI bool IntersectsObject(const Object& Object, const Ray& R)
{
	return IntersectsShape(Object.Shape, TransformRay(Object.Shape.InvTM, R));
}

HOST_DEVICE_NI bool IntersectsObject(const Ray& R)
{
	for (int i = 0; i < gpTracer->ObjectIDs.Count; i++)
	{
		if (IntersectsObject(gpObjects[i], R))
			return true;
	}
	
	return false;
}

}
