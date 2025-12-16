#include "main.h"
#include "game/game.h"
#include <cmath>

void CQuaternion::SetFromMatrix(RwMatrix* mat)
{
	w = sqrt( std::max( (float)0, 1.0f + mat->right.x + mat->up.y + mat->at.z ) ) * 0.5f;
	x = sqrt( std::max( (float)0, 1.0f + mat->right.x - mat->up.y - mat->at.z ) ) * 0.5f;
	y = sqrt( std::max( (float)0, 1.0f - mat->right.x + mat->up.y - mat->at.z ) ) * 0.5f;
	z = sqrt( std::max( (float)0, 1.0f - mat->right.x - mat->up.y + mat->at.z ) ) * 0.5f;

	x = copysignf(x, mat->at.y - mat->up.z);
	y = copysignf(y, mat->right.z - mat->at.x);
	z = copysignf(z, mat->up.x - mat->right.y);
}

void CQuaternion::GetMatrix(RwMatrix* mat)
{
	if(!mat) return;

	float sqw = w * w; // v13 = a1 * a1;
	float sqx = x * x; // v14 = a2 * a2;
	float sqy = y * y; // v15 = a3 * a3;
	float sqz = z * z; // v16 = a4 * a4;

	mat->right.x = ( sqx - sqy - sqz + sqw); 	// a5 = v14 - v15 - v16 + v13;
	mat->up.y = (-sqx + sqy - sqz + sqw);		// a9 = v15 - v14 - v16 + v13;
	mat->at.z = (-sqx - sqy + sqz + sqw);		// a13 = v16 - (v15 + v14) + v13;

	float tmp1 = x * y;				// v17 = a2 * a3;
	float tmp2 = z * w;				// v18 = a1 * a4;
	mat->up.x = 2.0 * (tmp1 + tmp2);		// a8 = v18 + v17 + v18 + v17;
	mat->right.y = 2.0 * (tmp1 - tmp2);		// a6 = v17 - v18 + v17 - v18;

	tmp1 = x*z;						// v20 = a2 * a4;
	tmp2 = y*w;						// v21 = a1 * a3;
	mat->at.x = 2.0 * (tmp1 - tmp2);		// a11 = v20 - v21 + v20 - v21;
	mat->right.z = 2.0 * (tmp1 + tmp2);		// a7 = v21 + v20 + v21 + v20;
	tmp1 = y*z;						// v22 = a3 * a4;
	tmp2 = x*w;						// v23 = a1 * a2;
	mat->at.y = 2.0 * (tmp1 + tmp2);		// a12 = v23 + v22 + v23 + v22;
	mat->up.z = 2.0 * (tmp1 - tmp2);		// a10 = v22 - v23 + v22 - v23;
}

void CQuaternion::Normalize()
{
	double n = sqrt(x*x + y*y + z*z + w*w);

	w /= n;
	x /= n;
	y /= n;
	z /= n;
}

void CQuaternion::Slerp(const CQuaternion *pQ1, const CQuaternion *pQ2, float t)
{
    if(!pQ1 || !pQ2) return;
    if(t <= 0.0f) { *this = *pQ1; return; }
    if(t >= 1.0f) { *this = *pQ2; return; }

    float x2 = pQ2->x, y2 = pQ2->y, z2 = pQ2->z, w2 = pQ2->w;
    float cosom = pQ1->x*x2 + pQ1->y*y2 + pQ1->z*z2 + pQ1->w*w2;

    // shortest path
    bool negate = false;
    if (cosom < 0.0f) { negate = true; cosom = -cosom; }

    // clamp to avoid NaN from acos
    cosom = std::clamp(cosom, -1.0f, 1.0f);

    const float THRESH = 0.9995f;
    if (cosom > THRESH) {
        // use nlerp for tiny angles (faster + stable)
        float inv = 1.0f - t;
        float bx = inv * pQ1->x + t * (negate ? -x2 : x2);
        float by = inv * pQ1->y + t * (negate ? -y2 : y2);
        float bz = inv * pQ1->z + t * (negate ? -z2 : z2);
        float bw = inv * pQ1->w + t * (negate ? -w2 : w2);
        // normalize
        float len = sqrtf(bx*bx + by*by + bz*bz + bw*bw);
        Set(bx/len, by/len, bz/len, bw/len);
        return;
    }

    double omega = acos(cosom);
    double sinom = sin(omega);
    double scale0 = sin((1.0 - t) * omega) / sinom;
    double scale1 = sin(t * omega) / sinom;
    if (negate) scale1 = -scale1;

    Set(
            (float)(scale0 * pQ1->x + scale1 * x2),
            (float)(scale0 * pQ1->y + scale1 * y2),
            (float)(scale0 * pQ1->z + scale1 * z2),
            (float)(scale0 * pQ1->w + scale1 * w2)
    );
}