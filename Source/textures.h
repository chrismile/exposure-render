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

#include "texture.h"

namespace ExposureRender
{

HOST_DEVICE ColorXYZf EvaluateProcedural(const Procedural& Procedural, const Vec2f& UVW)
{
	switch (Procedural.Type)
	{
		case Enums::Uniform:
			return ColorXYZf(Procedural.UniformColor);

		case Enums::Checker:
		{
			const int UV[2] =
			{
				(int)(UVW[0] * 2.0f),
				(int)(UVW[1] * 2.0f)
			};

			if (UV[0] % 2 == 0)
			{
				if (UV[1] % 2 == 0)
					return ColorXYZf(Procedural.CheckerColor1);
				else
					return ColorXYZf(Procedural.CheckerColor2);
			}
			else
			{
				if (UV[1] % 2 == 0)
					return ColorXYZf(Procedural.CheckerColor2);
				else
					return ColorXYZf(Procedural.CheckerColor1);
			}
		}

		case Enums::Gradient:
			return Procedural.Gradient.Evaluate(UVW[1]);
	}

	return ColorXYZf::Black();
}

HOST_DEVICE ColorXYZf EvaluateTexture(const int& ID, const Vec2f& UV)
{
	if (ID < 0)
		return ColorXYZf::Black();

	const Texture& T = gpTextures[ID];

	ColorXYZf L;

	Vec2f TextureUV = UV;

	TextureUV[0] *= T.Repeat[0];
	TextureUV[1] *= T.Repeat[1];
	
	TextureUV[0] += T.Offset[0];
	TextureUV[1] += 1.0f - T.Offset[1];
	
	TextureUV[0] = TextureUV[0] - floorf(TextureUV[0]);
	TextureUV[1] = TextureUV[1] - floorf(TextureUV[1]);

	TextureUV[0] = Clamp(TextureUV[0], 0.0f, 1.0f);
	TextureUV[1] = Clamp(TextureUV[1], 0.0f, 1.0f);

	if (T.Flip[0])
		TextureUV[0] = 1.0f - TextureUV[0];

	if (T.Flip[1])
		TextureUV[1] = 1.0f - TextureUV[1];

	switch (T.Type)
	{
		case Enums::Procedural:
		{
			L = EvaluateProcedural(T.Procedural, TextureUV);
			break;
		}

		case Enums::Bitmap:
		{
			if (T.BitmapID >= 0)
				L = ColorXYZf::FromRGBAuc(gpBitmaps[T.BitmapID].Pixels(TextureUV, true));

			break;
		}
	}

	return T.OutputLevel * L;
}

}
