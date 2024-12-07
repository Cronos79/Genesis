/******************************************************************************************
*	CronoGames Game Engine																  *
*	Copyright © 2024 CronoGames <http://www.cronogames.net>								  *
*																						  *
*	This file is part of CronoGames Game Engine.										  *
*																						  *
*	CronoGames Game Engine is free software: you can redistribute it and/or modify		  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The CronoGames Game Engine is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The CronoGames Game Engine.  If not, see <http://www.gnu.org/licenses/>.   *
******************************************************************************************/
#pragma once
#include <DirectXMath.h>
using namespace DirectX;

namespace Genesis
{
	class Camera
	{
	public:
		Camera();
		void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

		const XMMATRIX& GetViewMatrix() const;
		const XMMATRIX& GetProjectionMatrix() const;

		const XMVECTOR& GetPositionVector() const;
		const XMFLOAT3& GetPositionFloat3() const;
		const XMVECTOR& GetRotationVector() const;
		const XMFLOAT3& GetRotationFloat3() const;

		void SetPosition(const XMVECTOR& pos);
		void SetPosition(float x, float y, float z);
		void AdjustPosition(const XMVECTOR& pos);
		void AdjustPosition(float x, float y, float z);
		void SetRotation(const XMVECTOR& rot);
		void SetRotation(float x, float y, float z);
		void AdjustRotation(const XMVECTOR& rot);
		void AdjustRotation(float x, float y, float z);
		void SetLookAtPos(XMFLOAT3 lookAtPos);
		void SetLookAtPos(float x, float y, float z);

		const XMVECTOR& GetForwardVector();		
		const XMVECTOR& GetRightVector();
		const XMVECTOR& GetBackwardVector();
		const XMVECTOR& GetLeftVector();
	private:
		void UpdateViewMatrix();
		XMVECTOR m_posVector;
		XMVECTOR m_rotVector;
		XMFLOAT3 m_pos;
		XMFLOAT3 m_rot;
		XMMATRIX m_viewMatrix;
		XMMATRIX m_projectionMatrix;

		const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
		const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
		const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

		XMVECTOR m_forwardVector;
		XMVECTOR m_leftVector;
		XMVECTOR m_rightVector;
		XMVECTOR m_backwardVector;
	};
}
