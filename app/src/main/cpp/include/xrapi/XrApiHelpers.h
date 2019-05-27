//
// Created by SVR00003 on 2017/9/19.
//

#ifndef XRSDK_XRAPIHELPERS_H
#define XRSDK_XRAPIHELPERS_H


#include "math.h"		// for cosf(), sinf(), tanf()
#include "string.h"		// for memset()
#include "XrApiConfig.h"
#include "XrApiVersion.h"
#include "XrApiTypes.h"
#include <XrLogUtils.h>

#define XRAPI_PI		3.14159265358979323846f
#define XRAPI_ZNEAR		0.1f

#if defined( __GNUC__ )
#  define   XRAPI_UNUSED(a)   do {__typeof__ (&a) __attribute__ ((unused)) __tmp = &a; } while(0)
#else
#  define   XRAPI_UNUSED(a)   (a)
#endif

//-----------------------------------------------------------------
// Matrix helper functions.
//-----------------------------------------------------------------

// Use left-multiplication to accumulate transformations.
static inline xrMatrix4f xrMatrix4f_Multiply( const xrMatrix4f * a, const xrMatrix4f * b )
{
	xrMatrix4f out;
	out.M[0][0] = a->M[0][0] * b->M[0][0] + a->M[0][1] * b->M[1][0] + a->M[0][2] * b->M[2][0] + a->M[0][3] * b->M[3][0];
	out.M[1][0] = a->M[1][0] * b->M[0][0] + a->M[1][1] * b->M[1][0] + a->M[1][2] * b->M[2][0] + a->M[1][3] * b->M[3][0];
	out.M[2][0] = a->M[2][0] * b->M[0][0] + a->M[2][1] * b->M[1][0] + a->M[2][2] * b->M[2][0] + a->M[2][3] * b->M[3][0];
	out.M[3][0] = a->M[3][0] * b->M[0][0] + a->M[3][1] * b->M[1][0] + a->M[3][2] * b->M[2][0] + a->M[3][3] * b->M[3][0];

	out.M[0][1] = a->M[0][0] * b->M[0][1] + a->M[0][1] * b->M[1][1] + a->M[0][2] * b->M[2][1] + a->M[0][3] * b->M[3][1];
	out.M[1][1] = a->M[1][0] * b->M[0][1] + a->M[1][1] * b->M[1][1] + a->M[1][2] * b->M[2][1] + a->M[1][3] * b->M[3][1];
	out.M[2][1] = a->M[2][0] * b->M[0][1] + a->M[2][1] * b->M[1][1] + a->M[2][2] * b->M[2][1] + a->M[2][3] * b->M[3][1];
	out.M[3][1] = a->M[3][0] * b->M[0][1] + a->M[3][1] * b->M[1][1] + a->M[3][2] * b->M[2][1] + a->M[3][3] * b->M[3][1];

	out.M[0][2] = a->M[0][0] * b->M[0][2] + a->M[0][1] * b->M[1][2] + a->M[0][2] * b->M[2][2] + a->M[0][3] * b->M[3][2];
	out.M[1][2] = a->M[1][0] * b->M[0][2] + a->M[1][1] * b->M[1][2] + a->M[1][2] * b->M[2][2] + a->M[1][3] * b->M[3][2];
	out.M[2][2] = a->M[2][0] * b->M[0][2] + a->M[2][1] * b->M[1][2] + a->M[2][2] * b->M[2][2] + a->M[2][3] * b->M[3][2];
	out.M[3][2] = a->M[3][0] * b->M[0][2] + a->M[3][1] * b->M[1][2] + a->M[3][2] * b->M[2][2] + a->M[3][3] * b->M[3][2];

	out.M[0][3] = a->M[0][0] * b->M[0][3] + a->M[0][1] * b->M[1][3] + a->M[0][2] * b->M[2][3] + a->M[0][3] * b->M[3][3];
	out.M[1][3] = a->M[1][0] * b->M[0][3] + a->M[1][1] * b->M[1][3] + a->M[1][2] * b->M[2][3] + a->M[1][3] * b->M[3][3];
	out.M[2][3] = a->M[2][0] * b->M[0][3] + a->M[2][1] * b->M[1][3] + a->M[2][2] * b->M[2][3] + a->M[2][3] * b->M[3][3];
	out.M[3][3] = a->M[3][0] * b->M[0][3] + a->M[3][1] * b->M[1][3] + a->M[3][2] * b->M[2][3] + a->M[3][3] * b->M[3][3];
	return out;
}

// Returns the transpose of a 4x4 matrix.
static inline xrMatrix4f xrMatrix4f_Transpose( const xrMatrix4f * a )
{
	xrMatrix4f out;
	out.M[0][0] = a->M[0][0]; out.M[0][1] = a->M[1][0]; out.M[0][2] = a->M[2][0]; out.M[0][3] = a->M[3][0];
	out.M[1][0] = a->M[0][1]; out.M[1][1] = a->M[1][1]; out.M[1][2] = a->M[2][1]; out.M[1][3] = a->M[3][1];
	out.M[2][0] = a->M[0][2]; out.M[2][1] = a->M[1][2]; out.M[2][2] = a->M[2][2]; out.M[2][3] = a->M[3][2];
	out.M[3][0] = a->M[0][3]; out.M[3][1] = a->M[1][3]; out.M[3][2] = a->M[2][3]; out.M[3][3] = a->M[3][3];
	return out;
}

// Returns a 3x3 minor of a 4x4 matrix.
static inline float xrMatrix4f_Minor( const xrMatrix4f * m, int r0, int r1, int r2, int c0, int c1, int c2 )
{
	return	m->M[r0][c0] * ( m->M[r1][c1] * m->M[r2][c2] - m->M[r2][c1] * m->M[r1][c2] ) -
			m->M[r0][c1] * ( m->M[r1][c0] * m->M[r2][c2] - m->M[r2][c0] * m->M[r1][c2] ) +
			m->M[r0][c2] * ( m->M[r1][c0] * m->M[r2][c1] - m->M[r2][c0] * m->M[r1][c1] );
}

// Returns the inverse of a 4x4 matrix.
static inline xrMatrix4f xrMatrix4f_Inverse( const xrMatrix4f * m )
{
	const float rcpDet = 1.0f / (	m->M[0][0] * xrMatrix4f_Minor( m, 1, 2, 3, 1, 2, 3 ) -
									m->M[0][1] * xrMatrix4f_Minor( m, 1, 2, 3, 0, 2, 3 ) +
									m->M[0][2] * xrMatrix4f_Minor( m, 1, 2, 3, 0, 1, 3 ) -
									m->M[0][3] * xrMatrix4f_Minor( m, 1, 2, 3, 0, 1, 2 ) );
	xrMatrix4f out;
	out.M[0][0] =  xrMatrix4f_Minor( m, 1, 2, 3, 1, 2, 3 ) * rcpDet;
	out.M[0][1] = -xrMatrix4f_Minor( m, 0, 2, 3, 1, 2, 3 ) * rcpDet;
	out.M[0][2] =  xrMatrix4f_Minor( m, 0, 1, 3, 1, 2, 3 ) * rcpDet;
	out.M[0][3] = -xrMatrix4f_Minor( m, 0, 1, 2, 1, 2, 3 ) * rcpDet;
	out.M[1][0] = -xrMatrix4f_Minor( m, 1, 2, 3, 0, 2, 3 ) * rcpDet;
	out.M[1][1] =  xrMatrix4f_Minor( m, 0, 2, 3, 0, 2, 3 ) * rcpDet;
	out.M[1][2] = -xrMatrix4f_Minor( m, 0, 1, 3, 0, 2, 3 ) * rcpDet;
	out.M[1][3] =  xrMatrix4f_Minor( m, 0, 1, 2, 0, 2, 3 ) * rcpDet;
	out.M[2][0] =  xrMatrix4f_Minor( m, 1, 2, 3, 0, 1, 3 ) * rcpDet;
	out.M[2][1] = -xrMatrix4f_Minor( m, 0, 2, 3, 0, 1, 3 ) * rcpDet;
	out.M[2][2] =  xrMatrix4f_Minor( m, 0, 1, 3, 0, 1, 3 ) * rcpDet;
	out.M[2][3] = -xrMatrix4f_Minor( m, 0, 1, 2, 0, 1, 3 ) * rcpDet;
	out.M[3][0] = -xrMatrix4f_Minor( m, 1, 2, 3, 0, 1, 2 ) * rcpDet;
	out.M[3][1] =  xrMatrix4f_Minor( m, 0, 2, 3, 0, 1, 2 ) * rcpDet;
	out.M[3][2] = -xrMatrix4f_Minor( m, 0, 1, 3, 0, 1, 2 ) * rcpDet;
	out.M[3][3] =  xrMatrix4f_Minor( m, 0, 1, 2, 0, 1, 2 ) * rcpDet;
	return out;
}

// Returns a 4x4 identity matrix.
static inline xrMatrix4f xrMatrix4f_CreateIdentity()
{
	xrMatrix4f out;
	out.M[0][0] = 1.0f; out.M[0][1] = 0.0f; out.M[0][2] = 0.0f; out.M[0][3] = 0.0f;
	out.M[1][0] = 0.0f; out.M[1][1] = 1.0f; out.M[1][2] = 0.0f; out.M[1][3] = 0.0f;
	out.M[2][0] = 0.0f; out.M[2][1] = 0.0f; out.M[2][2] = 1.0f; out.M[2][3] = 0.0f;
	out.M[3][0] = 0.0f; out.M[3][1] = 0.0f; out.M[3][2] = 0.0f; out.M[3][3] = 1.0f;
	return out;
}

// Returns a 4x4 homogeneous translation matrix.
static inline xrMatrix4f xrMatrix4f_CreateTranslation( const float x, const float y, const float z )
{
	xrMatrix4f out;
	out.M[0][0] = 1.0f; out.M[0][1] = 0.0f; out.M[0][2] = 0.0f; out.M[0][3] = x;
	out.M[1][0] = 0.0f; out.M[1][1] = 1.0f; out.M[1][2] = 0.0f; out.M[1][3] = y;
	out.M[2][0] = 0.0f; out.M[2][1] = 0.0f; out.M[2][2] = 1.0f; out.M[2][3] = z;
	out.M[3][0] = 0.0f; out.M[3][1] = 0.0f; out.M[3][2] = 0.0f; out.M[3][3] = 1.0f;
	return out;
}

// Returns a 4x4 homogeneous rotation matrix.
static inline xrMatrix4f xrMatrix4f_CreateRotation( const float radiansX, const float radiansY, const float radiansZ )
{
	const float sinX = sinf( radiansX );
	const float cosX = cosf( radiansX );
	const xrMatrix4f rotationX =
	{ {
		{ 1,    0,     0, 0 },
		{ 0, cosX, -sinX, 0 },
		{ 0, sinX,  cosX, 0 },
		{ 0,    0,     0, 1 }
	} };
	const float sinY = sinf( radiansY );
	const float cosY = cosf( radiansY );
	const xrMatrix4f rotationY =
	{ {
		{  cosY, 0, sinY, 0 },
		{     0, 1,    0, 0 },
		{ -sinY, 0, cosY, 0 },
		{     0, 0,    0, 1 }
	} };
	const float sinZ = sinf( radiansZ );
	const float cosZ = cosf( radiansZ );
	const xrMatrix4f rotationZ =
	{ {
		{ cosZ, -sinZ, 0, 0 },
		{ sinZ,  cosZ, 0, 0 },
		{    0,     0, 1, 0 },
		{    0,     0, 0, 1 }
	} };
	const xrMatrix4f rotationXY = xrMatrix4f_Multiply( &rotationY, &rotationX );
	return xrMatrix4f_Multiply( &rotationZ, &rotationXY );
}

// Returns a projection matrix based on the specified dimensions.
// The projection matrix transforms -Z=forward, +Y=up, +X=right to the appropriate clip space for the graphics API.
// The far plane is placed at infinity if farZ <= nearZ.
// An infinite projection matrix is preferred for rasterization because, except for
// things *right* up against the near plane, it always provides better precision:
//		"Tightening the Precision of Perspective Rendering"
//		Paul Upchurch, Mathieu Desbrun
//		Journal of Graphics Tools, Volume 16, Issue 1, 2012
static inline xrMatrix4f xrMatrix4f_CreateProjection( const float minX, const float maxX,
											float const minY, const float maxY, const float nearZ, const float farZ )
{
	const float width = maxX - minX;
	const float height = maxY - minY;
	const float offsetZ = nearZ;	// set to zero for a [0,1] clip space

	xrMatrix4f out;
	if ( farZ <= nearZ )
	{
		// place the far plane at infinity
		out.M[0][0] = 2 * nearZ / width;
		out.M[0][1] = 0;
		out.M[0][2] = ( maxX + minX ) / width;
		out.M[0][3] = 0;

		out.M[1][0] = 0;
		out.M[1][1] = 2 * nearZ / height;
		out.M[1][2] = ( maxY + minY ) / height;
		out.M[1][3] = 0;

		out.M[2][0] = 0;
		out.M[2][1] = 0;
		out.M[2][2] = -1;
		out.M[2][3] = -( nearZ + offsetZ );

		out.M[3][0] = 0;
		out.M[3][1] = 0;
		out.M[3][2] = -1;
		out.M[3][3] = 0;
	}
	else
	{
		// normal projection
		out.M[0][0] = 2 * nearZ / width;
		out.M[0][1] = 0;
		out.M[0][2] = ( maxX + minX ) / width;
		out.M[0][3] = 0;

		out.M[1][0] = 0;
		out.M[1][1] = 2 * nearZ / height;
		out.M[1][2] = ( maxY + minY ) / height;
		out.M[1][3] = 0;

		out.M[2][0] = 0;
		out.M[2][1] = 0;
		out.M[2][2] = -( farZ + offsetZ ) / ( farZ - nearZ );
		out.M[2][3] = -( farZ * ( nearZ + offsetZ ) ) / ( farZ - nearZ );

		out.M[3][0] = 0;
		out.M[3][1] = 0;
		out.M[3][2] = -1;
		out.M[3][3] = 0;
	}
	return out;
}

// Returns a projection matrix based on the given FOV.
static inline xrMatrix4f xrMatrix4f_CreateProjectionFov( const float fovDegreesX, const float fovDegreesY,
												const float offsetX, const float offsetY, const float nearZ, const float farZ )
{
	const float halfWidth = nearZ * tanf( fovDegreesX * ( XRAPI_PI / 180.0f * 0.5f ) );
	const float halfHeight = nearZ * tanf( fovDegreesY * ( XRAPI_PI / 180.0f * 0.5f ) );

	const float minX = offsetX - halfWidth;
	const float maxX = offsetX + halfWidth;

	const float minY = offsetY - halfHeight;
	const float maxY = offsetY + halfHeight;

	return xrMatrix4f_CreateProjection( minX, maxX, minY, maxY, nearZ, farZ );
}

// Returns the 4x4 rotation matrix for the given quaternion.
static inline xrMatrix4f xrMatrix4f_CreateFromQuaternion( const xrQuatf * q )
{
	const float ww = q->w * q->w;
	const float xx = q->x * q->x;
	const float yy = q->y * q->y;
	const float zz = q->z * q->z;

	xrMatrix4f out;
	out.M[0][0] = ww + xx - yy - zz;
	out.M[0][1] = 2 * ( q->x * q->y - q->w * q->z );
	out.M[0][2] = 2 * ( q->x * q->z + q->w * q->y );
	out.M[0][3] = 0;

	out.M[1][0] = 2 * ( q->x * q->y + q->w * q->z );
	out.M[1][1] = ww - xx + yy - zz;
	out.M[1][2] = 2 * ( q->y * q->z - q->w * q->x );
	out.M[1][3] = 0;

	out.M[2][0] = 2 * ( q->x * q->z - q->w * q->y );
	out.M[2][1] = 2 * ( q->y * q->z + q->w * q->x );
	out.M[2][2] = ww - xx - yy + zz;
	out.M[2][3] = 0;

	out.M[3][0] = 0;
	out.M[3][1] = 0;
	out.M[3][2] = 0;
	out.M[3][3] = 1;
	return out;
}

// Convert a standard projection matrix into a TexCoordsFromTanAngles matrix for
// the primary time warp surface.
static inline xrMatrix4f xrMatrix4f_TanAngleMatrixFromProjection( const xrMatrix4f * projection )
{
	/*
		A projection matrix goes from a view point to NDC, or -1 to 1 space.
		Scale and bias to convert that to a 0 to 1 space.

		const xrMatrix3f m =
		{ {
			{ projection->M[0][0],                0.0f, projection->M[0][2] },
			{                0.0f, projection->M[1][1], projection->M[1][2] },
			{                0.0f,                0.0f,               -1.0f }
		} };
		// Note that there is no Y-flip because eye buffers have 0,0 = left-bottom.
		const xrMatrix3f s = xrMatrix3f_CreateScaling( 0.5f, 0.5f );
		const xrMatrix3f t = xrMatrix3f_CreateTranslation( 0.5f, 0.5f );
		const xrMatrix3f r0 = xrMatrix3f_Multiply( &s, &m );
		const xrMatrix3f r1 = xrMatrix3f_Multiply( &t, &r0 );
		return r1;

		clipZ = ( z * projection[2][2] + projection[2][3] ) / ( projection[3][2] * z )
		z = projection[2][3] / ( clipZ * projection[3][2] - projection[2][2] )
		z = ( projection[2][3] / projection[3][2] ) / ( clipZ - projection[2][2] / projection[3][2] )
	*/
	const xrMatrix4f tanAngleMatrix =
	{ {
		{ 0.5f * projection->M[0][0], 0.0f, 0.5f * projection->M[0][2] - 0.5f, 0.0f },
		{ 0.0f, 0.5f * projection->M[1][1], 0.5f * projection->M[1][2] - 0.5f, 0.0f },
		{ 0.0f, 0.0f, -1.0f, 0.0f },
		// Store the values to convert a clip-Z to a linear depth in the unused matrix elements.
		{ projection->M[2][2], projection->M[2][3], projection->M[3][2], 1.0f }
	} };
	return tanAngleMatrix;
}

// If a simple quad defined as a -1 to 1 XY unit square is transformed to
// the camera view with the given modelView matrix, it can alternately be
// drawn as a time warp overlay image to take advantage of the full window
// resolution, which is usually higher than the eye buffer textures, and
// avoids resampling both into the eye buffer, and again to the screen.
// This is used for high quality movie screens and user interface planes.
//
// Note that this is NOT an MVP matrix -- the "projection" is handled
// by the distortion process.
//
// This utility functions converts a model-view matrix that would normally
// draw a -1 to 1 unit square to the view into a TexCoordsFromTanAngles matrix
// for an overlay surface.
//
// The resulting z value should be straight ahead distance to the plane.
// The x and y values will be pre-multiplied by z for projective texturing.
static inline xrMatrix4f xrMatrix4f_TanAngleMatrixFromUnitSquare( const xrMatrix4f * modelView )
{
	/*
		// Take the inverse of the view matrix because the view matrix transforms the unit square
		// from world space into view space, while the matrix needed here is the one that transforms
		// the unit square from view space to world space.
		const xrMatrix4f inv = xrMatrix4f_Inverse( modelView );
		// This matrix calculates the projection onto the (-1, 1) X and Y axes of the unit square,
		// of the intersection of the vector (tanX, tanY, -1) with the plane described by the matrix
		// that transforms the unit square into world space.
		const xrMatrix3f m =
		{ {
			{	inv.M[0][0] * inv.M[2][3] - inv.M[0][3] * inv.M[2][0],
				inv.M[0][1] * inv.M[2][3] - inv.M[0][3] * inv.M[2][1],
				inv.M[0][2] * inv.M[2][3] - inv.M[0][3] * inv.M[2][2] },
			{	inv.M[1][0] * inv.M[2][3] - inv.M[1][3] * inv.M[2][0],
				inv.M[1][1] * inv.M[2][3] - inv.M[1][3] * inv.M[2][1],
				inv.M[1][2] * inv.M[2][3] - inv.M[1][3] * inv.M[2][2] },
			{	- inv.M[2][0],
				- inv.M[2][1],
				- inv.M[2][2] }
		} };
		// Flip the Y because textures have 0,0 = left-top as opposed to left-bottom.
		const xrMatrix3f f = xrMatrix3f_CreateScaling( 1.0f, -1.0f );
		const xrMatrix3f s = xrMatrix3f_CreateScaling( 0.5f, 0.5f );
		const xrMatrix3f t = xrMatrix3f_CreateTranslation( 0.5f, 0.5f );
		const xrMatrix3f r0 = xrMatrix3f_Multiply( &f, &m );
		const xrMatrix3f r1 = xrMatrix3f_Multiply( &s, &r0 );
		const xrMatrix3f r2 = xrMatrix3f_Multiply( &t, &r1 );
		return r2;
	*/

	const xrMatrix4f inv = xrMatrix4f_Inverse( modelView );
	const float coef = ( inv.M[2][3] > 0.0f ) ? 1.0f : -1.0f;

	xrMatrix4f m;
	m.M[0][0] = ( +0.5f * ( inv.M[0][0] * inv.M[2][3] - inv.M[0][3] * inv.M[2][0] ) - 0.5f * inv.M[2][0] ) * coef;
	m.M[0][1] = ( +0.5f * ( inv.M[0][1] * inv.M[2][3] - inv.M[0][3] * inv.M[2][1] ) - 0.5f * inv.M[2][1] ) * coef;
	m.M[0][2] = ( +0.5f * ( inv.M[0][2] * inv.M[2][3] - inv.M[0][3] * inv.M[2][2] ) - 0.5f * inv.M[2][2] ) * coef;
	m.M[0][3] = 0.0f;

	m.M[1][0] = ( -0.5f * ( inv.M[1][0] * inv.M[2][3] - inv.M[1][3] * inv.M[2][0] ) - 0.5f * inv.M[2][0] ) * coef;
	m.M[1][1] = ( -0.5f * ( inv.M[1][1] * inv.M[2][3] - inv.M[1][3] * inv.M[2][1] ) - 0.5f * inv.M[2][1] ) * coef;
	m.M[1][2] = ( -0.5f * ( inv.M[1][2] * inv.M[2][3] - inv.M[1][3] * inv.M[2][2] ) - 0.5f * inv.M[2][2] ) * coef;
	m.M[1][3] = 0.0f;

	m.M[2][0] = ( -inv.M[2][0] ) * coef;
	m.M[2][1] = ( -inv.M[2][1] ) * coef;
	m.M[2][2] = ( -inv.M[2][2] ) * coef;
	m.M[2][3] = 0.0f;

	m.M[3][0] = 0.0f;
	m.M[3][1] = 0.0f;
	m.M[3][2] = 0.0f;
	m.M[3][3] = 1.0f;
	return m;
}

// Convert a standard view matrix into a TexCoordsFromTanAngles matrix for
// the looking into a cube map.
static inline xrMatrix4f xrMatrix4f_TanAngleMatrixForCubeMap( const xrMatrix4f * viewMatrix )
{
    xrMatrix4f m = *viewMatrix;
    // clear translation
    for ( int i = 0; i < 3; i++ )
    {
        m.M[ i ][ 3 ] = 0.0f;
    }
    return xrMatrix4f_Inverse( &m );
}

// Utility function to calculate external velocity for smooth stick yaw turning.
// To reduce judder in FPS style experiences when the application framerate is
// lower than the vsync rate, the rotation from a joypad can be applied to the
// view space distorted eye vectors before applying the time warp.
static inline xrMatrix4f xrMatrix4f_CalculateExternalVelocity( const xrMatrix4f * viewMatrix, const float yawRadiansPerSecond )
{
	const float angle = yawRadiansPerSecond * ( -1.0f / 60.0f );
	const float sinHalfAngle = sinf( angle * 0.5f );
	const float cosHalfAngle = cosf( angle * 0.5f );

	// Yaw is always going to be around the world Y axis
	xrQuatf quat;
	quat.x = viewMatrix->M[0][1] * sinHalfAngle;
	quat.y = viewMatrix->M[1][1] * sinHalfAngle;
	quat.z = viewMatrix->M[2][1] * sinHalfAngle;
	quat.w = cosHalfAngle;
	return xrMatrix4f_CreateFromQuaternion( &quat );
}

//-----------------------------------------------------------------
// Default initialization helper functions.
//-----------------------------------------------------------------

// Utility function to default initialize the xrInitParms.
static inline xrInitParms xrapiDefaultInitParms( const xrJava * java )
{
	xrInitParms parms;
	memset( &parms, 0, sizeof( parms ) );

	parms.Type = XRAPI_STRUCTURE_TYPE_INIT_PARMS;
	parms.ProductVersion = XRAPI_PRODUCT_VERSION;
	parms.MajorVersion = XRAPI_MAJOR_VERSION;
	parms.MinorVersion = XRAPI_MINOR_VERSION;
	parms.PatchVersion = XRAPI_PATCH_VERSION;
	parms.GraphicsAPI = XRAPI_GRAPHICS_API_OPENGL_ES_2;
	parms.Java = *java;

	return parms;
}

// Utility function to default initialize the xrModeParms.
static inline xrModeParms xrapiDefaultModeParms( const xrJava * java )
{
	xrModeParms parms;
	memset( &parms, 0, sizeof( parms ) );

	parms.Type = XRAPI_STRUCTURE_TYPE_MODE_PARMS;
	parms.Flags |= XRAPI_MODE_FLAG_ALLOW_POWER_SAVE;
	parms.Flags |= XRAPI_MODE_FLAG_RESET_WINDOW_FULLSCREEN;
	parms.Java = *java;

	return parms;
}

// Utility function to default initialize the xrPerformanceParms.
static inline xrPerformanceParms xrapiDefaultPerformanceParms()
{
	xrPerformanceParms parms;
	parms.CpuLevel = 2;
	parms.GpuLevel = 2;
	parms.MainThreadTid = 0;
	parms.RenderThreadTid = 0;
	return parms;
}

typedef enum
{
	XRAPI_FRAME_INIT_DEFAULT,
	XRAPI_FRAME_INIT_BLACK,
	XRAPI_FRAME_INIT_BLACK_FLUSH,
	XRAPI_FRAME_INIT_BLACK_FINAL,
	XRAPI_FRAME_INIT_LOADING_ICON,
	XRAPI_FRAME_INIT_LOADING_ICON_FLUSH,
	XRAPI_FRAME_INIT_MESSAGE,
	XRAPI_FRAME_INIT_MESSAGE_FLUSH
} xrFrameInit;

// Utility function to default initialize the xrFrameParms.
static inline xrFrameParms xrapiDefaultFrameParms( const xrJava * java, const xrFrameInit init, const double currentTime,
													 xrTextureSwapChain * textureSwapChain )
{
	const xrMatrix4f projectionMatrix = xrMatrix4f_CreateProjectionFov( 90.0f, 90.0f, 0.0f, 0.0f, 0.1f, 0.0f );
	const xrMatrix4f texCoordsFromTanAngles = xrMatrix4f_TanAngleMatrixFromProjection( &projectionMatrix );

	xrFrameParms parms;
	memset( &parms, 0, sizeof( parms ) );

	parms.Type = XRAPI_STRUCTURE_TYPE_FRAME_PARMS;
	for ( int layer = 0; layer < XRAPI_FRAME_LAYER_TYPE_MAX; layer++ )
	{
		parms.Layers[layer].ColorScale = 1.0f;
		for ( int eye = 0; eye < XRAPI_FRAME_LAYER_EYE_MAX; eye++ )
		{
			parms.Layers[layer].Textures[eye].TexCoordsFromTanAngles = texCoordsFromTanAngles;
			parms.Layers[layer].Textures[eye].TextureRect.width = 1.0f;
			parms.Layers[layer].Textures[eye].TextureRect.height = 1.0f;
			parms.Layers[layer].Textures[eye].HeadPose.Pose.Orientation.w = 1.0f;
			parms.Layers[layer].Textures[eye].HeadPose.TimeInSeconds = currentTime;
		}
	}
	parms.LayerCount = 1;
	parms.MinimumVsyncs = 1;
	parms.ExtraLatencyMode = XRAPI_EXTRA_LATENCY_MODE_OFF;
	parms.ExternalVelocity.M[0][0] = 1.0f;
	parms.ExternalVelocity.M[1][1] = 1.0f;
	parms.ExternalVelocity.M[2][2] = 1.0f;
	parms.ExternalVelocity.M[3][3] = 1.0f;
	parms.PerformanceParms = xrapiDefaultPerformanceParms();
	parms.Java = *java;

	parms.Layers[0].SrcBlend = XRAPI_FRAME_LAYER_BLEND_ONE;
	parms.Layers[0].DstBlend = XRAPI_FRAME_LAYER_BLEND_ZERO;
	parms.Layers[0].Flags = 0;

	parms.Layers[1].SrcBlend = XRAPI_FRAME_LAYER_BLEND_SRC_ALPHA;
	parms.Layers[1].DstBlend = XRAPI_FRAME_LAYER_BLEND_ONE_MINUS_SRC_ALPHA;
	parms.Layers[1].Flags = 0;

	switch ( init )
	{
		case XRAPI_FRAME_INIT_DEFAULT:
		{
			break;
		}
		case XRAPI_FRAME_INIT_BLACK:
		case XRAPI_FRAME_INIT_BLACK_FLUSH:
		case XRAPI_FRAME_INIT_BLACK_FINAL:
		{
			parms.Flags = XRAPI_FRAME_FLAG_INHIBIT_SRGB_FRAMEBUFFER;
			for ( int eye = 0; eye < XRAPI_FRAME_LAYER_EYE_MAX; eye++ )
			{
				parms.Layers[0].Textures[eye].ColorTextureSwapChain = (xrTextureSwapChain *)XRAPI_DEFAULT_TEXTURE_SWAPCHAIN_BLACK;
			}
			break;
		}
		case XRAPI_FRAME_INIT_LOADING_ICON:
		case XRAPI_FRAME_INIT_LOADING_ICON_FLUSH:
		{
			parms.LayerCount = 2;
			parms.Flags = XRAPI_FRAME_FLAG_INHIBIT_SRGB_FRAMEBUFFER;
			parms.Layers[1].Flags = XRAPI_FRAME_LAYER_FLAG_SPIN;
			parms.Layers[1].SpinSpeed = 1.0f;		// rotation in radians per second
			parms.Layers[1].SpinScale = 16.0f;		// icon size factor smaller than fullscreen
			for ( int eye = 0; eye < XRAPI_FRAME_LAYER_EYE_MAX; eye++ )
			{
				parms.Layers[0].Textures[eye].ColorTextureSwapChain = (xrTextureSwapChain *)XRAPI_DEFAULT_TEXTURE_SWAPCHAIN_BLACK;
				parms.Layers[1].Textures[eye].ColorTextureSwapChain = ( textureSwapChain != NULL ) ? textureSwapChain : (xrTextureSwapChain *)XRAPI_DEFAULT_TEXTURE_SWAPCHAIN_LOADING_ICON;
			}
			break;
		}
		case XRAPI_FRAME_INIT_MESSAGE:
		case XRAPI_FRAME_INIT_MESSAGE_FLUSH:
		{
			parms.LayerCount = 2;
			parms.Flags = XRAPI_FRAME_FLAG_INHIBIT_SRGB_FRAMEBUFFER;
			parms.Layers[1].SpinSpeed = 0.0f;		// rotation in radians per second
			parms.Layers[1].SpinScale = 2.0f;		// message size factor smaller than fullscreen
			for ( int eye = 0; eye < XRAPI_FRAME_LAYER_EYE_MAX; eye++ )
			{
				parms.Layers[0].Textures[eye].ColorTextureSwapChain = (xrTextureSwapChain *)XRAPI_DEFAULT_TEXTURE_SWAPCHAIN_BLACK;
				parms.Layers[1].Textures[eye].ColorTextureSwapChain = ( textureSwapChain != NULL ) ? textureSwapChain : (xrTextureSwapChain *)XRAPI_DEFAULT_TEXTURE_SWAPCHAIN_LOADING_ICON;
			}
			break;
		}
	}

	if ( init == XRAPI_FRAME_INIT_BLACK_FLUSH || init == XRAPI_FRAME_INIT_LOADING_ICON_FLUSH || init == XRAPI_FRAME_INIT_MESSAGE_FLUSH )
	{
		parms.Flags |= XRAPI_FRAME_FLAG_FLUSH;
	}
	if ( init == XRAPI_FRAME_INIT_BLACK_FINAL )
	{
		parms.Flags |= XRAPI_FRAME_FLAG_FLUSH | XRAPI_FRAME_FLAG_FINAL;
	}

	return parms;
}


//-----------------------------------------------------------------
// Head Model
//-----------------------------------------------------------------

// Utility function to default initialize the xrHeadModelParms.
static inline xrHeadModelParms xrapiDefaultHeadModelParms()
{
	xrHeadModelParms parms;
	memset( &parms, 0, sizeof( parms ) );

	parms.InterpupillaryDistance	= 0.0640f;	// average interpupillary distance
	parms.EyeHeight					= 1.6750f;	// average eye height above the ground when standing
	parms.HeadModelDepth			= 0.0805f;
	parms.HeadModelHeight			= 0.0750f;

	return parms;
}

//-----------------------------------------------------------------
// Eye view matrix helper functions.
//-----------------------------------------------------------------

// Apply the head-on-a-stick model if head tracking is not available.
static inline xrTracking xrapiApplyHeadModel( const xrHeadModelParms * headModelParms, const xrTracking * tracking )
{
	if ( ( tracking->Status & XRAPI_TRACKING_STATUS_POSITION_TRACKED ) == 0 )
	{
		// Calculate the head position based on the head orientation using a head-on-a-stick model.
		const xrHeadModelParms * p = headModelParms;
		const xrMatrix4f m = xrMatrix4f_CreateFromQuaternion( &tracking->HeadPose.Pose.Orientation );
		xrTracking newTracking = *tracking;
		newTracking.HeadPose.Pose.Position.x = m.M[0][1] * p->HeadModelHeight - m.M[0][2] * p->HeadModelDepth;
		newTracking.HeadPose.Pose.Position.y = m.M[1][1] * p->HeadModelHeight - m.M[1][2] * p->HeadModelDepth - p->HeadModelHeight;// + p->EyeHeight;
		newTracking.HeadPose.Pose.Position.z = m.M[2][1] * p->HeadModelHeight - m.M[2][2] * p->HeadModelDepth;
		return newTracking;
	}
	return *tracking;
}


static inline xrMatrix4f xrapiGetTransformFromPose( const xrPosef * pose )
{
	const xrMatrix4f rotation = xrMatrix4f_CreateFromQuaternion( &pose->Orientation );
	const xrMatrix4f translation = xrMatrix4f_CreateTranslation( pose->Position.x, pose->Position.y, pose->Position.z );
	return xrMatrix4f_Multiply( &translation, &rotation );
}

static inline xrMatrix4f xrapiGetViewMatrixFromPose( const xrPosef * pose )
{
	const xrMatrix4f transform = xrapiGetTransformFromPose( pose );
	return xrMatrix4f_Inverse( &transform );
}

// Utility function to get the eye view matrix based on the center eye view matrix and the IPD.
static inline xrMatrix4f xrapiGetEyeViewMatrix(	const xrHeadModelParms * headModelParms,
													const xrMatrix4f * centerEyeViewMatrix,
													const int eye )
{
	const float eyeOffset = ( eye ? -0.5f : 0.5f ) * headModelParms->InterpupillaryDistance;
	const xrMatrix4f eyeOffsetMatrix = xrMatrix4f_CreateTranslation( eyeOffset, 0.0f, 0.0f );
	return xrMatrix4f_Multiply( &eyeOffsetMatrix, centerEyeViewMatrix );
}
static inline void xrMatrix4f_Log(const char* prefix, const xrMatrix4f mat) {
#if defined(LOG_MAT)
	LOGD("%s:%f %f %f %f,%f %f %f %f,%f %f %f %f,%f %f %f %f,",prefix,
		 mat.M[0][0], mat.M[0][1], mat.M[0][2], mat.M[0][3],
		 mat.M[1][0], mat.M[1][1], mat.M[1][2], mat.M[1][3],
		 mat.M[2][0], mat.M[2][1], mat.M[2][2], mat.M[2][3],
		 mat.M[3][0], mat.M[3][1], mat.M[3][2], mat.M[3][3]);
#endif
}
#endif //XRSDK_XRAPIHELPERS_H
