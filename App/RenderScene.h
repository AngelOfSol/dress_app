#pragma once
#include "sceneid.h"
#include "glm_includes.h"
#include "Cylinder.h"
#include "AxisMesh.h"
#include "ConcavePolygonMesh.h"
#include "cylinderdata.h"
#include "ToolTipIndicator.h"
#include "Button.h"
#include "TextButton.h"
#include "ACRConnection.h"
class RenderScene :
	public SceneID<RenderScene>
{
public:
	RenderScene(OpenGLHandler& parent, cylinder_data&);
	~RenderScene(void);
	void init(void) override;
	scene_id update(int) override;
	void draw(void) override;
private:
	void toRealtime();
	void updateRealtime();
	void toAnimation();
	void updateAnimation(int_frac);

	cylinder_data& input_;

	float zNear;
	float zFar;
	float fovY;

	glm::mat4 projectionMatrix;
	glm::mat4 worldMat;
	glm::mat4 rotateMat;
	struct
	{
		glm::fquat x_axis;
		glm::fquat y_axis;
	} orient;
	double x_axis;
	GLfloat zoom;

	double z_distance;
	double y_distance;

	int_frac speedMultiplier;

	std::vector<Move> moves;
	std::vector<Move> movesCopy;

	Cylinder model;
	Cylinder finalModel;
	AxisMesh reference;
	ConcavePolygonMesh minimap;
	ToolTipIndicator indicator;
	ConcavePolygonMesh home_button;
	ConcavePolygonMesh arrow_button;

	int resolution;

	glm::vec4 diffuseColor;
	std::vector<glm::vec4> lightSources;
	GLfloat ambient;
	GLfloat attenuation;
	double lineAlpha;

	glm::vec2 previousMouse;

	int_frac secondsPast;
	int_frac progressScalar;
	int_frac unit_scale;

	vec2d tooltipPos;

	TextButton::ptr replayButton;
	TextButton::ptr toRealtimeButton;
	TextButton::ptr toAnimationButton;
	TextButton::ptr measureRadiusButton;
	TextButton::ptr measureDifferentialButton;
	Button::ptr upButton;
	Button::ptr downButton;
	Button::ptr leftButton;
	Button::ptr rightButton;
	Button::ptr resetButton;

	float dxRotation;
	float dyRotation;

	double profileZoom;

	struct key_repeat
	{
		bool flag = false;
		double progress = 0.0;
		void reset()
		{
			this->flag = false;
			this->progress = 0.0;
		}
	} key_a, key_d;

	struct
	{
		bool on;
		double x_origin;
	} radius_differential;

	bool zero_moves;

	enum
	{
		NONE,
		PLAYING,
		REALTIME
	} state;

	ACRConnection::ptr realtimeAccess;

	void draw_stencil(glm::mat4 s, const ConcavePolygonMesh& ref, glm::vec4 color = glm::vec4());
};