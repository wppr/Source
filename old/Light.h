#ifndef LIGHT_H
#define LIGHT_H
#include "MyString.h"
#include "Movable.h"
#include "Mathlib.h"
#include "NameGenerator.h"
namespace HW
{
	/** light position is always (0,0,0) in local space
	light direction is always (-1,-1,-1) in local space.
	*/
	class SceneManager;
	class Camera;
	struct light_struct{
		float position[3];
		float maxdist;
		float intensity[3];
		float exponent;
		float direction[3];
		float cutoff;
		
		float shadowmat[16];
	};
	class Light : public Movable
	{
	public:
		enum LightType
		{
			LT_UNKNOWN = 0,
			LT_POINT = 1,
			LT_DIRECTIONAL = 2,
			LT_SPOT = 3
		};
		/** default constructor,should not be used by user.
		*/
		Light();
		/** constructor with name and type.
		@remarks
		the name and type of the instance should not be modified once created.
		*/
		Light(const String &name, LightType type, SceneManager * m_creator = NULL);
		/** default destructor
		*/
		virtual ~Light(){}
		/** return the type of this light
		*/
		LightType getLightType() const { return m_Type; }
		/** return the maximum distance of this light
		*/
		float    getMaxDistance() { return m_MaximumDistance; }
		// set light position

		void setPosition(Vector3 pos)
		{
			if (pos == Position)
				return;
			Position = pos;
			m_moved = true;
			m_camera_valid = false;
	
		}
		Vector3 getPosition()
		{
			return Position;
		}

		void setDirection(Vector3 Dir)
		{
			Dir.normalize();
			if (Dir == Direction)
				return;
			Direction = Dir;
			m_moved = true;
			m_camera_valid = false;
		

		}

		Vector3 getDirection()
		{
			return Direction;
		}

		/** set the distance
		*/
		void     SetMaxDistance(float dist) { m_MaximumDistance = dist; }
		/** set type of this light.this function should be used carefully.
		the type of a light should not be modified once set.
		*/
		float calcMaxdistance();
		void    SetType(LightType type) { m_Type = type; }

		/**check is this light turn o
		*/
		bool isLightOn()
		{
			return mState;
		}
		/** set light state
		*/
		void setLight(bool state)
		{
			mState = state;
		}
		/** check if this light cast shadow
		*/
		bool isCastShadow()
		{
			return mCastShadow;
		}
		/** set light cast shadow or not
		*/
		void setCastShadow(bool castshadow)
		{
			mCastShadow = castshadow;
		}
		void CalcScissorRect(Camera* camera, float r,Vector4& rect);

		Matrix4 getShadowMatrix();
		Camera* getCamera();
		void completeMove()
		{
			m_moved = false;
		}
		void CollectData(vector<Light*> lights, void*& p, int& structsize, int& number);
		void CollectData(void*& p, int& structsize);
		void CollectData(vector<Light*> lights, vector<float>& p, int& structsize, const String& name);
	protected:
		/** action to be taken when this object is moved.
		*/
		virtual void moveActionImp()
		{
			m_moved = true;
		}
		virtual void getBoundingBoxImp()
		{
			m_boundingBox.setNull();
		}
		virtual void getBoundingSphereImp()
		{
			m_sphere.setRadius(0);
		}
	public:
		/** color intensity
		*/
		Vector4  Diffuse;
		Vector4  Specular;

		/** attenuation for point light and spot light
		*/
		float    ConstantAttenuation;
		float    LinearAttenuation;
		float    QuadraticAttenuation;

		/** position of the light ,it is valid for point light
		and spot light.
		*/

		float   Exponent;
		/** and angle used by spot light
		*/
		float   Cutoff;

		/** ShadowMap Matrix.only useful for directional and spotlight
		*/
		Matrix4 ShadowMatrix;

		/** ShadowMap Bias
		*/
		float ShadowBias;
		Camera* m_camera;
		bool m_camera_valid;
		bool m_shadowmatrix_valid;
		
	private:
		/** type of this light
		*/
		LightType m_Type;
		/** maximum distance to be influenced by this light
		*/
		float   m_MaximumDistance;
		/** Light on or off
		*/
		bool mState;
		/** light cast shadow or not
		*/
		bool mCastShadow;

		Vector3  Position;
		/** direction of the light,it is valid for directional light
		and spot light.
		*/
		Vector3 Direction;
		/** for spot light
		*/

		/** name generator
		*/
		static NameGenerator* m_NameGenerator;
	};

}
#endif