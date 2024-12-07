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
#include "Camera.h"

namespace Genesis
{
	Camera::Camera()
	{
		m_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_posVector = XMLoadFloat3(&m_pos);
		m_rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_rotVector = XMLoadFloat3(&m_rot);
		UpdateViewMatrix();
	}

	void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
	{
		float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
		m_projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
	}

	const XMMATRIX& Camera::GetViewMatrix() const
	{
		return m_viewMatrix;
	}

	const XMMATRIX& Camera::GetProjectionMatrix() const
	{
		return m_projectionMatrix;
	}

	const XMVECTOR& Camera::GetPositionVector() const
	{
		return m_posVector;
	}

	const XMFLOAT3& Camera::GetPositionFloat3() const
	{
		return m_pos;
	}

	const XMVECTOR& Camera::GetRotationVector() const
	{
		return m_rotVector;
	}

	const XMFLOAT3& Camera::GetRotationFloat3() const
	{
		return m_rot;
	}

	void Camera::SetPosition(const XMVECTOR& pos)
	{
		XMStoreFloat3(&m_pos, pos);
		m_posVector = pos;
		UpdateViewMatrix();
	}

	void Camera::SetPosition(float x, float y, float z)
	{
		m_pos = XMFLOAT3(x, y, z);
		m_posVector = XMLoadFloat3(&m_pos);
		UpdateViewMatrix();
	}

	void Camera::AdjustPosition(const XMVECTOR& pos)
	{
		m_posVector += pos;
		XMStoreFloat3(&m_pos, m_posVector);
		UpdateViewMatrix();
	}

	void Camera::AdjustPosition(float x, float y, float z)
	{
		m_pos.x += x;
		m_pos.y += y;
		m_pos.z += z;
		m_posVector = XMLoadFloat3(&m_pos);
		UpdateViewMatrix();
	}

	void Camera::SetRotation(const XMVECTOR& rot)
	{
		m_rotVector = rot;
		XMStoreFloat3(&m_rot, rot);
		UpdateViewMatrix();
	}

	void Camera::SetRotation(float x, float y, float z)
	{
		m_rot = XMFLOAT3(x, y, z);
		m_rotVector = XMLoadFloat3(&m_rot);
		UpdateViewMatrix();
	}

	void Camera::AdjustRotation(const XMVECTOR& rot)
	{
		m_rotVector += rot;
		XMStoreFloat3(&m_rot, m_rotVector);
		UpdateViewMatrix();
	}

	void Camera::AdjustRotation(float x, float y, float z)
	{
		m_rot.x += x;
		m_rot.y += y;
		m_rot.z += z;
		m_rotVector = XMLoadFloat3(&m_rot);
		UpdateViewMatrix();
	}

	void Camera::SetLookAtPos(XMFLOAT3 lookAtPos)
	{
		if (lookAtPos.x == m_pos.x && lookAtPos.y == m_pos.y && lookAtPos.z == m_pos.z)
		{
			return;
		}

		lookAtPos.x = m_pos.x - lookAtPos.x;
		lookAtPos.y = m_pos.y - lookAtPos.y;
		lookAtPos.z = m_pos.z - lookAtPos.z;

		float pitch = 0.0f;
		if (lookAtPos.y != 0.0f)
		{
			const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
			pitch = atan(lookAtPos.y / distance);
		}

		float yaw = 0.0f;
		if (lookAtPos.x != 0.0f)
		{
			yaw = atan(lookAtPos.x / lookAtPos.z);
		}

		if (lookAtPos.z > 0)
		{
			yaw += XM_PI;
		}

		SetRotation(pitch, yaw, 0.0f);
	}

	void Camera::SetLookAtPos(float x, float y, float z)
	{
		XMFLOAT3 lookAtPos = XMFLOAT3(x, y, z);
		SetLookAtPos(lookAtPos);
	}

	const XMVECTOR& Camera::GetForwardVector()
	{
		return m_forwardVector;
	}

	const DirectX::XMVECTOR& Camera::GetRightVector()
	{
		return m_rightVector;
	}

	const XMVECTOR& Camera::GetBackwardVector()
	{
		return m_backwardVector;
	}

	const XMVECTOR& Camera::GetLeftVector()
	{
		return m_leftVector;
	}

	void Camera::UpdateViewMatrix() //Updates view matrix and also updates the movement vectors
	{
		//Calculate camera rotation matrix
		XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(m_rot.x, m_rot.y, m_rot.z);
		//Calculate unit vector of cam target based off camera forward value transformed by cam rotation matrix
		XMVECTOR camTarget = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, camRotationMatrix);
		//Adjust cam target to be offset by the camera's current position
		camTarget += m_posVector;
		//Calculate up direction based on current rotation
		XMVECTOR upDir = XMVector3TransformCoord(DEFAULT_UP_VECTOR, camRotationMatrix);
		//Rebuild view matrix
		m_viewMatrix = XMMatrixLookAtLH(m_posVector, camTarget, upDir);

		//Update movement vectors
		XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, m_rot.y, 0.0f);
		m_forwardVector = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
		m_backwardVector = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
		m_rightVector = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
		m_leftVector = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	}
}
