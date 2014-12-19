#include "RenderScene.h"
#include "OpenGLHandler.h"
#include "glm_includes.h"
#include <fstream>
#include "MatrixStack.h"
#include "state_gl.h"
#include "segment_reader.h"
#include "make_poly.h"
#include <algorithm>
#include "Config.h"
#include "file_loc.h"
#include "split_string.h"

RenderScene::RenderScene(OpenGLHandler& parent, cylinder_data& data) :SceneID(parent), input_(data)
{
}

RenderScene::~RenderScene(void)
{
}

void RenderScene::init(void)
{
	Config& config = *this->input_.config;

	if (this->input_.acr)
		this->realtimeAccess = make_connection("192.168.10.40");//config.as<std::string>("ip"));

	this->speedMultiplier = config.as<int_frac>("speed");

	auto movdata = loadMoves(this->input_.fileName);

	this->unit_scale = int_frac(1);
	if (!movdata.inches)
		this->unit_scale = int_frac(10) / int_frac(254);

	for (auto& m : movdata.moves)
	{
		m.speed *= this->unit_scale;
		m.center *= this->unit_scale;
		m.end *= this->unit_scale;
		m.start *= this->unit_scale;
	}

	auto radius = int_frac(movdata.diameter) / int_frac(2.0) * this->unit_scale;
	auto length = int_frac(movdata.length) * this->unit_scale;
	auto x_start = int_frac(movdata.x_start) * this->unit_scale;
	auto z_start = int_frac(movdata.z_start) * this->unit_scale;

	this->input_.size.y = radius;
	this->input_.size.x = length;
	this->input_.toolHome.x = z_start;
	this->input_.toolHome.y = x_start;
	this->input_.unitType = units::inches;
	if (!movdata.inches)
		this->input_.unitType = units::millimeters;


	this->moves = movdata.moves;

	auto invalids = 0;
	for (auto move : this->moves)
	{
		if (move.id == MoveType::INVALID)
			invalids++;
	}
	this->zero_moves = false;
	if (this->moves.size() - invalids == 0)
		this->zero_moves = true;

	this->input_.toolType = tools::triangle;

	switch (this->input_.toolType)
	{
	case tools::invalid:
		break;
	case tools::rounded:

		this->finalModel.turtleShape() = this->model.turtleShape() = make_circle_tip(config.as<int_frac>("circle-tip-radius"), config.as<int_frac>("circle-tip-height"), 18);
		break;
	case tools::triangle:
		this->finalModel.turtleShape() = this->model.turtleShape() = make_iso_triangle(vec2d(config.as<int_frac>("triangle-tip-width"), config.as<int_frac>("triangle-tip-height")));
		break;
	case tools::rectangle:
		this->finalModel.turtleShape() = this->model.turtleShape() = make_rectangle(vec2d(config.as<int_frac>("triangle-tip-width"), config.as<int_frac>("triangle-tip-height")), vec2d(0,0));
		break;
	}


	this->finalModel.reference().x = this->model.reference().x = 0;
	if (movdata.exit)
	{
		this->finalModel.reference().x = this->model.reference().x = length;;
	}
	this->finalModel.reference().y = this->model.reference().y = radius;
	/*
		this->model.reference().y = radius;
		switch (this->input_.side)
		{
		case origin_side::invalid:
		break;
		case origin_side::operator_:
		this->finalModel.reference() = this->model.reference().x = 0;
		break;
		case origin_side::exit:
		this->finalModel.reference() = this->model.reference().x = length;
		break;
		}*/

	//this->finalModel.reference() = this->model.reference() += this->input_.offset;


	this->finalModel.baseLine() = this->model.baseLine() = stor<base_int>(config.as<std::string>("hole-radius"));
	this->finalModel.initialSize() = this->model.initialSize() = vec2d(length, radius);
	this->finalModel.initialTurtle() = this->model.initialTurtle() = vec2d(z_start, x_start);
	this->finalModel.initialTurtle() = this->model.initialTurtle() += this->model.reference();

	this->model.buildInitial();
	this->finalModel.buildInitial();

	this->finalModel.scale().y = this->model.scale().y = stor<base_int>(config.as<std::string>("y-scale"));


	for (auto m : this->moves)
	{
		this->finalModel.pushMove(m, int_frac(1));
	}

	this->movesCopy = this->moves;

	this->finalModel.rebuildMesh();
	this->model.rebuildMesh();

	this->lineAlpha = 0.67f;

	auto fixed = this->model.fixMove(this->moves.front());
	this->progressScalar = fixed.end.length() / fixed.speed.length();
	if (fixed.end.length() == 0)
		this->progressScalar = 0;

	// build the tool tip indicator

	this->indicator.pushVertex(ttVertex());

	this->indicator.buildMesh();

	// build the reference axes	

	this->reference.pushVertex(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec4(1.0f, 0.0f, 0.0f, this->lineAlpha),
		glm::vec4(0.0f, 1.0f, 0.0f, this->lineAlpha),
		glm::vec4(0.0f, 0.0f, 1.0f, this->lineAlpha),
		2.0f
		);

	this->reference.buildMesh();


	// set up some graphics values

	this->ambient = 0.2f;
	this->attenuation = (float)this->input_.size.y;
	this->diffuseColor = glm::vec4(config.as<double>("model-color-r"), config.as<double>("model-color-g"), config.as<double>("model-color-b"), 1.0f);
	this->resolution = config.as<int>("model-res");
	this->zoom = 0.9f;

	// create the lightsources


	for (auto i = 0; i <= int(this->input_.size.x) + 1; i++)
	{
		auto rad_p1 = 1 * ((float)this->input_.size.y + 1.4f);
		glm::vec4 light{ i, rad_p1, rad_p1, 1 };
		this->lightSources.push_back(light);

	}

	/*this->lightSources.push_back(glm::vec4(3.0f, (float)this->input_.size.y + 1.0f, (float)this->input_.size.y + 1.0f, 1.0f));
	this->lightSources.push_back(glm::vec4(1.3f, (float)this->input_.size.y + 1.0f, (float)this->input_.size.y + 1.0f, 1.0f));
	this->lightSources.push_back(glm::vec4(-1.3f, (float)this->input_.size.y + 1.0f, (float)this->input_.size.y + 1.0f, 1.0f));
	this->lightSources.push_back(glm::vec4(-3.0f, (float)this->input_.size.y + 1.0f, (float)this->input_.size.y + 1.0f, 1.0f));
	*/
	// set up some uniforms

	this->gl_.useProgram("cylinderSweep");

	this->gl_.set(glUniform1i, "resolution", this->resolution);
	this->gl_.set(glUniform1i, "numLights", this->lightSources.size());
	this->gl_.set(glUniform1f, "ambient", this->ambient);
	this->gl_.set(glUniform1f, "attenuation", this->attenuation);
	this->gl_.set(glUniform4fv, "lightSource", this->lightSources.size(), &(this->lightSources.data()[0][0]));
	this->gl_.set(glUniform4fv, "diffuse", 1, &this->diffuseColor[0]);

	this->secondsPast = 0;
	// calculate the projection matrixyeah

	this->zNear = 1.0f;
	this->zFar = 200.0f;

	this->fovY = glm::radians(45.0f);

	float asr = this->gl_.windowSize().y / this->gl_.windowSize().x;

	this->projectionMatrix = glm::perspective(this->fovY, asr, this->zNear, this->zFar);

	this->rotateMat = glm::mat4();

	// set some initial positio values

	this->z_distance = -9.0 * (double)this->model.initialSize().y;
	this->y_distance = 0.0;
	this->x_axis = -this->model.initialSize().x / int_frac(2.0f);
	this->state = PLAYING;

	this->replayButton = make_textbutton(this->gl_, "courier", "PLAY", glm::vec2(10, 225));
	this->replayButton->onPress() = [this](bool val)
	{
		if (!val)
		{
			this->toAnimation();
		}
	};

	this->toRealtimeButton = make_textbutton(this->gl_, "courier", "REAL TIME", glm::vec2(10, 30));
	this->toRealtimeButton->onPress() = [this](bool val)
	{
		if (!val)
		{
			this->toRealtime();
		}
	};
	this->toAnimationButton = make_textbutton(this->gl_, "courier", "ANIMATION", glm::vec2(10, 30));
	this->toAnimationButton->onPress() = [this](bool val)
	{
		if (!val)
		{
			this->toAnimation();
		}
	};

	this->measureRadiusButton = make_textbutton(this->gl_, "courier", "DIFFERENTIAL", glm::vec2(10, 80));
	this->measureRadiusButton->onPress() = [this](bool val)
	{
		if (!val)
		{
			this->radius_differential.on = true;
			this->radius_differential.x_origin = this->x_axis;
		}
	};
	this->measureDifferentialButton = make_textbutton(this->gl_, "courier", "RADIUS", glm::vec2(10, 80));
	this->measureDifferentialButton->onPress() = [this](bool val)
	{
		if (!val)
		{
			this->radius_differential.on = false;
		}
	};

	glm::vec2 center(50, 160);
	glm::vec2 size(25, 25);
	this->upButton = make_button(this->gl_, glm::vec2(0, 30) + center, size);
	this->downButton = make_button(this->gl_, glm::vec2(0, -30) + center, size);
	this->rightButton = make_button(this->gl_, glm::vec2(30, 0) + center, size);
	this->leftButton = make_button(this->gl_, glm::vec2(-30, 0) + center, size);
	this->resetButton = make_button(this->gl_, glm::vec2(0, 0) + center, size);


	this->upButton->onPress() = [this](bool pressed)
	{
		if (pressed)
		{
			this->downButton->off();
		}
	};

	this->downButton->onPress() = [this](bool pressed)
	{
		if (pressed)
		{
			this->upButton->off();
		}
	};

	this->leftButton->onPress() = [this](bool pressed)
	{
		if (pressed)
		{
			this->rightButton->off();
		}
	};

	this->rightButton->onPress() = [this](bool pressed)
	{
		if (pressed)
		{
			this->leftButton->off();
		}
	};

	this->resetButton->onPress() = [this](bool pressed)
	{
		if (pressed)
		{
			if (this->upButton->pressed()
				|| this->downButton->pressed()
				|| this->rightButton->pressed()
				|| this->leftButton->pressed())
			{
				this->upButton->off();
				this->downButton->off();
				this->rightButton->off();
				this->leftButton->off();
			}
			else
			{
				this->orient.x_axis = glm::fquat();
				this->orient.y_axis = glm::fquat();
				this->y_distance = 0;
				this->z_distance = -9.0 * (double)this->model.initialSize().y;
				this->x_axis = -this->model.initialSize().x / int_frac(2.0f);;
				this->profileZoom = 1.0f;
			}

		}


	};
	this->dxRotation = 0;
	this->dyRotation = 0;

	this->profileZoom = 1.0f;

	this->key_a.reset();
	this->key_d.reset();

	this->radius_differential.on = false;


	this->arrow_button.pushVertex({ 0, 0.5 });
	this->arrow_button.pushVertex({ -0.5, -0.5 });
	this->arrow_button.pushVertex({ 0.0, -0.3 });
	this->arrow_button.pushVertex({ 0.5, -0.5 });

	this->arrow_button.buildMesh();

	this->home_button.pushVertex({ 0, 1 });
	this->home_button.pushVertex({ 1.2, 0 });
	this->home_button.pushVertex({ 0.8, 0 });
	this->home_button.pushVertex({ 0.8, -1 });
	this->home_button.pushVertex({ -0.8, -1 });
	this->home_button.pushVertex({ -0.8, 0 });
	this->home_button.pushVertex({ -1.2, 0 });

	this->home_button.buildMesh();
}
void RenderScene::toAnimation()
{
	this->state = PLAYING;
	this->model.buildInitial();
	this->moves = this->movesCopy;
}
void RenderScene::toRealtime()
{
	this->state = REALTIME;
	auto copy = this->model.initialTurtle();
	// figure out where to put starting turtle
	this->model.buildInitial();
	this->model.initialTurtle() = copy;
}
scene_id RenderScene::update(int elapsed)
{
	this->upButton->update(elapsed);
	this->downButton->update(elapsed);
	this->rightButton->update(elapsed);
	this->leftButton->update(elapsed);
	this->resetButton->update(elapsed);

	if (!this->radius_differential.on)
	{
		this->measureRadiusButton->update(elapsed);
	}
	else
	{
		this->measureDifferentialButton->update(elapsed);
	}


	auto perSecond = int_frac(elapsed, 1000);


	auto rotate = [perSecond](glm::vec3 axis, float change)
	{
		// get the change in mouse position

		// convert the change in x to a change in the y axis angle in radians
		float axisRotation = change * 3.14f / 180 / 2;

		// 2.0f is just a small scaling value
		axis *= sinf(axisRotation) / 2.0f;
		float scalar = cosf(axisRotation / 2.0f);

		// create a quaternion from the axis and the scalar
		return glm::fquat(scalar, axis.x, axis.y, axis.z);
	};

	// combine the previous two quaternions with the current orientation


	float rotationSpeed = 85 * (float)perSecond;

	if (this->upButton->pressed() || this->gl_.keys().up)
		this->dyRotation = -rotationSpeed;
	else if (this->downButton->pressed() || this->gl_.keys().down)
		this->dyRotation = rotationSpeed;
	else
		this->dyRotation = 0;

	if (this->rightButton->pressed() || this->gl_.keys().right)
		this->dxRotation = rotationSpeed;
	else if (this->leftButton->pressed() || this->gl_.keys().left)
		this->dxRotation = -rotationSpeed;
	else
		this->dxRotation = 0;

	this->orient.x_axis = glm::normalize(rotate(glm::vec3(0, 1, 0), -dxRotation)) * this->orient.x_axis;
	this->orient.x_axis = glm::normalize(this->orient.x_axis);


	this->orient.y_axis = glm::normalize(rotate(glm::vec3(1, 0, 0), -dyRotation)) * this->orient.y_axis;
	this->orient.y_axis = glm::normalize(this->orient.y_axis);


	//glm::normalize(this->orient);
	//this->orient = glm::normalize(rotate(glm::vec3(0, 1, 0), dxRotation)) * this->orient;
	//this->orient = glm::normalize(this->orient);

	//this->orient = glm::normalize(rotate(glm::vec3(1, 0, 0), dyRotation)) * this->orient;
	//this->orient = glm::normalize(this->orient);

	auto orient = this->orient.y_axis * this->orient.x_axis;

	// cast the quarternion back to a rotation matrix
	this->rotateMat = glm::mat4_cast(orient);

	if (this->state == REALTIME)
	{
		this->toAnimationButton->update(elapsed);
	}
	else
	{
		this->toRealtimeButton->update(elapsed);
	}



	if (this->state == PLAYING && this->moves.size() != 0)
	{
		this->updateAnimation(perSecond);
	}
	else if (this->state == REALTIME)
	{
		this->updateRealtime();
	}
	else
	{
		this->state = NONE;
		this->replayButton->update(elapsed);
	}

	// calculate the screen aspect ratio
	float asr = this->gl_.windowSize().x / this->gl_.windowSize().y;

	this->projectionMatrix = glm::perspective(this->fovY, asr, this->zNear, this->zFar);

	this->gl_.useProgram("cylinderSweep");
	this->gl_.set(glUniform1i, "resolution", this->resolution);

	if (this->gl_.keys()['e'])
	{
		this->z_distance *= (double)(int_frac(1.0f) + (int_frac(0.5f) * perSecond));
	}
	else if (this->gl_.keys()['q'])
	{
		this->z_distance /= (double)(int_frac(1.0f) + (int_frac(0.5f) * perSecond));
	}

	if (!this->gl_.keys().shift)
	{
		this->key_a.reset();
		this->key_d.reset();

		auto speed = (double)(int_frac(2.0f) * perSecond);

		if (this->gl_.keys().control)
		{
			speed *= 0.01;
		}

		if (this->gl_.keys()['a'])
		{
			this->x_axis -= speed;
		}
		else if (this->gl_.keys()['d'])
		{
			this->x_axis += speed;
		}
	}
	else
	{

		if (this->gl_.keys()['a'] && !this->key_a.flag)
		{
			this->key_a.flag = true;

			this->x_axis -= (double)(int_frac(1) / int_frac(10000));

		}
		else if (!this->gl_.keys()['a'] && this->key_a.flag)
		{
			this->key_a.reset();
		}
		else if (this->gl_.keys()['a'] && this->key_a.flag)
		{

			this->key_a.progress += (double)perSecond;

			if (this->key_a.progress >= 0.3)
			{
				this->key_a.reset();
			}
		}

		if (this->gl_.keys()['d'] && !this->key_d.flag)
		{
			this->key_d.flag = true;
			this->x_axis += (double)(int_frac(1) / int_frac(10000));

		}
		else if (!this->gl_.keys()['d'] && this->key_d.flag)
		{
			this->key_d.reset();
		}
		else if (this->gl_.keys()['d'] && this->key_d.flag)
		{

			this->key_d.progress += (double)perSecond;
			if (this->key_d.progress >= 0.3)
			{
				this->key_d.reset();
			}
		}
	}

	if (this->gl_.keys()['s'])
	{
		this->y_distance -= (double)(int_frac(3) * perSecond);
	}
	else if (this->gl_.keys()['w'])
	{
		this->y_distance += (double)(int_frac(3) * perSecond);
	}

	if (this->gl_.keys()['z'])
	{
		this->profileZoom *= (double)(int_frac(1.0f) + (int_frac(0.5f) * perSecond));
		this->profileZoom = std::min(2.0, this->profileZoom);
	}
	else if (this->gl_.keys()['x'])
	{
		this->profileZoom /= (double)(int_frac(1.0f) + (int_frac(0.5f) * perSecond));
	}

	if (this->gl_.keys().space)
	{
		this->toAnimation();
	}
	if (this->gl_.keys().enter)
	{
		this->toRealtime();
	}
	if (this->gl_.keys()['p'])
	{
		this->gl_.close();
	}

	if (this->gl_.keys()['m'])
	{
		this->radius_differential.on = true;
		this->radius_differential.x_origin = this->x_axis;
	}
	else if (this->gl_.keys()['n'])
	{
		this->radius_differential.on = false;
	}

	if (this->input_.pipe->check())
	{
		std::string input = this->input_.pipe->get();

		if (input == "close")
			this->gl_.close();
		if (input.substr(0, 4) == "move")
		{
			input = input.substr(5);
			auto nums = split(input, ' ');
			int x = std::stoi(nums[0]);
			int y = std::stoi(nums[1]);
			int w = std::stoi(nums[2]);
			int h = std::stoi(nums[3]);

			SetWindowPos(this->input_.hwnd, HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW);
		}

	}
	SetWindowPos(this->input_.hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);

	this->minimap.clear();

	auto& verts = (this->state == REALTIME) ? this->finalModel.vertexes() : this->model.vertexes();
	for (auto temp : verts)
	{
		this->minimap.pushVertex(temp);
	}
	this->minimap.buildMesh();

	return this->id;
}
void RenderScene::updateAnimation(int_frac perSecond)
{
	this->secondsPast += perSecond;

	auto progress = this->progressScalar * this->secondsPast * this->speedMultiplier;

	if (this->progressScalar == 0)
		progress = 1;

	if (progress < 0)
		progress = 0;

	if (progress > 1)
		progress = 1;


	this->model.pushMove(this->moves.front(), progress);
	this->model.rebuildMesh();

	if (progress == 1)
	{
		this->moves.erase(this->moves.begin(), this->moves.begin() + 1);
		this->secondsPast = 0;
		if (this->moves.size() != 0)
		{
			if (this->moves.front().id == MoveType::CIRCLE)
			{
				auto move = this->moves.front();
				auto startVec = move.start - move.center;
				auto endVec = move.end - move.center;

				auto radius = startVec.length();
				auto startAngle = atan(startVec);
				auto endAngle = atan(endVec);
				auto changeAngle = endAngle - startAngle;

				auto distance = radius * changeAngle;

				this->progressScalar = abs(move.speed.length() / distance);
			}
			else
			{
				auto fixed = this->model.fixMove(this->moves.front());
				this->progressScalar = fixed.speed.length() / fixed.end.length();
				if (fixed.end.length() == 0)
					this->progressScalar = 0;
			}
		}
		else
		{
			this->state = NONE;
		}
	}

}
void RenderScene::updateRealtime()
{
	if (this->realtimeAccess)
	{
		vec2d new_pos;
		new_pos.x = this->realtimeAccess->getLong(13314);
		new_pos.y = this->realtimeAccess->getLong(13570);

		new_pos /= 254000;

		this->tooltipPos = new_pos;
	}
}
void RenderScene::draw(void)
{
	/*
	propagate unit_scale
	*/

	const auto& currentModel = (this->state == REALTIME) ? this->finalModel : this->model;
	vec2d turtle = currentModel.turtle();
	if (this->state == REALTIME)
	{
		turtle = this->tooltipPos;
	}




	GLStateOn<GL_DEPTH_TEST> depth;

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glDepthRange(0.0f, 1.0f);
	// clear the screen and the depth buffer

	glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// create a matrix stack
	rmg_com::MatrixStack stack;

	auto updateUniform = [this, &stack]()
	{
		this->gl_.set(glUniformMatrix4fv, "worldMat", 1, false, &(stack.get()[0][0]));
	};

	// apply viewport transformations
	auto viewportTransform = this->projectionMatrix // projection matrix
		* glm::translate(glm::vec3(0.0f, 0.0f, this->z_distance)
		); // zoom in and out
	{
		rmg_com::Transform viewport(stack, viewportTransform);

		// move the cylinder mesh along th x-axis for view
		// rotate the models
		{
			rmg_com::Transform worldRotate(stack, this->rotateMat);

			{
				rmg_com::Transform modelSpace(stack, glm::translate(glm::vec3(this->x_axis, this->y_distance, 0.0f)));

				// render the radius of the cylinder
				this->gl_.useProgram("cylinderSweep");
				updateUniform();
				currentModel.mesh().draw(GL_LINE_STRIP);


				{
					GLStateOff<GL_STENCIL_TEST> depthOff;


					rmg_com::Transform indicator(stack, glm::translate(glm::vec3((glm::vec2)turtle, 0.0f)));

					this->gl_.useProgram("indicator");
					updateUniform();
					glm::vec4 color(0.7, 0.2, 0.2, 1.0);
					this->gl_.set(glUniform4fv, "pColor", 1, &color[0]);

					this->indicator.draw(GL_POINTS);

				}

			}


			// draw the parts of the axes that lie outside the cylinder
			this->gl_.useProgram("axisDraw");
			updateUniform();
			glLineWidth(6.0f);
			this->reference.draw(GL_LINES);

			// swap to blend for rendering the parts of the axes in the cylinder

			{
				GLStateOn<GL_BLEND> blend;
				GLStateOff<GL_DEPTH_TEST> depthOff;

				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				glLineWidth(1.0f);
				this->reference.draw(GL_LINES);

			}
		}
	}
	auto uiTransform = glm::ortho(0.0f, this->gl_.windowSize().x, 0.0f, this->gl_.windowSize().y)
		;
	{
		rmg_com::Transform ui(stack, uiTransform);

		{

			rmg_com::Transform minimapTransform(stack,
				glm::translate(glm::vec3(this->gl_.windowSize().x / 4 * 3, this->gl_.windowSize().y / 4 * 3, 0))
				* glm::scale(glm::vec3(90.0f * this->profileZoom, 90.0f * this->profileZoom, 1)));
			{

				rmg_com::Transform transformSecond(stack,
					glm::scale(glm::vec3(1 / (float)(this->input_.size.y - currentModel.baseLine()), 1 / (float)(this->input_.size.y - currentModel.baseLine()), 1))
					* glm::translate(glm::vec3(this->x_axis, std::min(-(double)currentModel.baseLine(), this->y_distance), 0)));
				// disable the depth test, because we're rendering the stencil
				GLStateOff<GL_DEPTH_TEST> depthOff;
				GLStateOn<GL_STENCIL_TEST> stencilOn;

				// disable color and depth, so we don't draw to them
				// while drawing to the stencil
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
				glDepthMask(GL_FALSE);

				// fail whenever theres a pixel written there
				glStencilFunc(GL_NEVER, 0, 0);
				// on fail, invert the stencil buffer
				// essentially allowing us to count even-odd of triangle overlap
				// it should never pass, but if it does we leave it be
				// inversion makes it either 0 or non-zero, which ever it wasn't before
				glStencilOp(GL_INVERT, GL_KEEP, GL_KEEP);

				// sets all of the mask to write enabled
				// so we can write to the whole thing
				glStencilMask(0xFF);
				// clear the stencil buffer to 0
				glClear(GL_STENCIL_BUFFER_BIT);

				// render all of the triangles to the stencil buffer
				// so we can perform our even odd calculations

				this->gl_.useProgram("concaveRender");
				updateUniform();
				auto concColor = glm::vec4{ 0.4f, 0.4f, 0.4f, 1.0f };
				this->gl_.set(glUniform4fv, "color", 1, &concColor[0]);
				this->minimap.draw(GL_TRIANGLE_FAN);

				// we've gotten our mask so we can start rendering to the screen now
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				glDepthMask(GL_TRUE);

				// make the buffer write-protected
				glStencilMask(0x00);

				// if the buffer isn't 0, that means we should draw there
				// as its an odd fill number
				glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
				this->minimap.draw(GL_TRIANGLE_FAN);


				GLStateOff<GL_STENCIL_TEST> stencilOff;
				rmg_com::Transform indicator(stack,
					glm::translate(glm::vec3((glm::vec2)turtle, 0.0f)));

				this->gl_.useProgram("indicator");
				updateUniform();
				glm::vec4 color(0.7, 0.2, 0.2, 1.0);
				this->gl_.set(glUniform4fv, "pColor", 1, &color[0]);

				this->indicator.draw(GL_POINTS);



			}
			GLStateOff<GL_DEPTH_TEST, GL_STENCIL_TEST> depthOff;
			rmg_com::Transform axisRetransform(stack,
				glm::scale(glm::vec3(0.5, 0.5, 1)));
			this->gl_.useProgram("axisDraw");
			updateUniform();
			glLineWidth(3.0f);
			this->reference.draw(GL_LINES);

		}
		{
			this->gl_.useFont("courier");
			glm::vec2 message_bar_pos(0, 5);
			auto fix_str = [](double f)
			{
				std::stringstream ss;
				ss << std::setprecision(7) << std::fixed;
				ss << f;
				return ss.str();
			};
			if (!this->zero_moves && (this->state != REALTIME || (this->state == REALTIME && this->realtimeAccess)))
			{

				if (!this->radius_differential.on)
				{
					auto rad = currentModel.radiusAt(-this->x_axis);
					rad /= this->unit_scale;
					this->gl_.write(
						"Radius: "
						+ fix_str(rad)
						+ " "
						+ (this->input_.unitType == units::inches ? "in" : "mm")
						+ " @ x: "
						+ std::to_string(-this->x_axis)
						, message_bar_pos);
				}
				else
				{
					auto diff = currentModel.radiusAt(-this->x_axis) - currentModel.radiusAt(-this->radius_differential.x_origin);
					diff /= this->unit_scale;
					this->gl_.write(
						"Diff: "
						+ fix_str(diff)
						+ " "
						+ (this->input_.unitType == units::inches ? "in" : "mm")
						+ " @ x: "
						+ std::to_string(-this->x_axis)
						, message_bar_pos);
				}
			}
			else
			{
				if (state != REALTIME)
					this->gl_.write(
					"There seem to be no machine move commands in your recipe."
					, message_bar_pos);
				else
					this->gl_.write("There seems to be no ACR connection."
					, message_bar_pos);
			}
		}
	}
	if (this->state == REALTIME)
	{
		this->toAnimationButton->draw();
	}
	else
	{
		this->toRealtimeButton->draw();
	}
	if (!this->radius_differential.on)
	{
		this->measureRadiusButton->draw();
	}
	else
	{
		this->measureDifferentialButton->draw();
	}
	{
	}


	auto scale_mat = glm::scale(glm::vec3{ 24, 24, 1 });

	auto ortho_mat = glm::ortho(0.0f, this->gl_.windowSize().x, 0.0f, this->gl_.windowSize().y);

	auto size_offset_mat = glm::translate(glm::vec3(this->upButton->size() / 2.0f, 0.0f));

	auto pos_mat_of = [](glm::vec2 v) { return glm::translate(glm::vec3(v, 0)); };

	auto rot_mat_of = [](float rot) { return glm::rotate(rot, glm::vec3{ 0, 0, 1 }); };

	auto draw_arrow = [&](GUIElementID<Button>::ptr but, float rot, glm::vec4 color)
	{
		this->draw_stencil(
			ortho_mat
			* pos_mat_of(but->pos())
			* size_offset_mat
			* rot_mat_of(rot)
			* scale_mat
			, this->arrow_button, color);
	};

	auto button_off_color = glm::vec4{ 0.6f, 0.6f, 0.6f, 0.0f };

	draw_arrow(this->upButton, 0, button_off_color);

	draw_arrow(this->downButton, 3.14159f, button_off_color);

	draw_arrow(this->leftButton, 3.14159f / 2, button_off_color);
	draw_arrow(this->rightButton, 3.14159f * 3 / 2, button_off_color);


	this->draw_stencil(
		ortho_mat
		* pos_mat_of(this->resetButton->pos() + glm::vec2{ 0, 2 })
		* size_offset_mat
		* glm::scale(glm::vec3{ 14, 14, 1 })
		, this->home_button, button_off_color);

	if (this->state == NONE)
		this->replayButton->draw();
	/*
	this->upButton->draw();
	this->downButton->draw();
	this->rightButton->draw();
	this->leftButton->draw();*/
	/*
	
	*/


	//this->resetButton->draw();
	// draw everything here
}

void RenderScene::draw_stencil(glm::mat4 s, const ConcavePolygonMesh& ref, glm::vec4 color)
{
	// disable the depth test, because we're rendering the stencil
	GLStateOff<GL_DEPTH_TEST> depthOff;
	GLStateOn<GL_STENCIL_TEST> stencilOn;

	// disable color and depth, so we don't draw to them
	// while drawing to the stencil
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	// fail whenever theres a pixel written there
	glStencilFunc(GL_NEVER, 0, 0);
	// on fail, invert the stencil buffer
	// essentially allowing us to count even-odd of triangle overlap
	// it should never pass, but if it does we leave it be
	// inversion makes it either 0 or non-zero, which ever it wasn't before
	glStencilOp(GL_INVERT, GL_KEEP, GL_KEEP);

	// sets all of the mask to write enabled
	// so we can write to the whole thing
	glStencilMask(0xFF);
	// clear the stencil buffer to 0
	glClear(GL_STENCIL_BUFFER_BIT);
	this->gl_.useProgram("concaveRender");
	this->gl_.set(glUniformMatrix4fv, "worldMat", 1, false, &(s[0][0]));
	this->gl_.set(glUniform4fv, "color", 1, &(color[0]));
	ref.draw(GL_TRIANGLE_FAN);

	// we've gotten our mask so we can start rendering to the screen now
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

	// make the buffer write-protected
	glStencilMask(0x00);

	// if the buffer isn't 0, that means we should draw there
	// as its an odd fill number
	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
	ref.draw(GL_TRIANGLE_FAN);

}