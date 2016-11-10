#pragma once
#include "Camera.h"
#include "mathlib.h"
#include "Interpolation.h"

using namespace HW;

class CameraController {
public:
	float PitchSpeed;
	float YawSpeed;
	float LinearSpeed;
	float Acceleration;
	CameraController() {}
	CameraController(double pitchSpeed, double yawSpeed, float linearSpeed, float acceleration)
		:PitchSpeed(pitchSpeed), YawSpeed(yawSpeed), LinearSpeed(linearSpeed), Acceleration(acceleration){}
	void UpdateCamera(Camera* camera, double t) {
		camera->RotatePitchYaw(t*PitchSpeed, t*YawSpeed);
		Vector3 Pos = camera->getPosition();
		Vector3 Dir = camera->getDirection();
		Dir.normalize();
		LinearSpeed += Acceleration*t;
		Pos += LinearSpeed*t*Dir;
		camera->setPosition(Pos);
	}
};

struct InterpData {
	float Time;
	Vector3 Position;
	Vector3 Direction;

	InterpData(float time, Vector3 position, Vector3 direction)
		:Time(time), Position(position), Direction(direction) {}
};

class Pattern {
public:
	Pattern() {}
	Pattern(const vector<InterpData>& datas) {
		setMarks(datas);
	}

	void setMarks(const vector<InterpData>& datas) {
		int n = datas.size();
		vector<double> times(n, 0);
		vector<Vector3> positions(n), directions(n);
		for (int i = 0; i < n; i++) {
			times[i] = datas[i].Time;
			positions[i] = datas[i].Position;
			directions[i] = datas[i].Direction;
		}

		PosInterp.SetData(times, positions);
		DirInterp.SetData(times, directions);
		hasDir = true;
	}

	Vector3 getPosition(double t) {
		return PosInterp.GetInterpolation(t);
	}

	Vector3 getDirection(double t) {
		if (hasDir)
			return DirInterp.GetInterpolation(t);
		else
			return PosInterp.GetDerivative(t);
	}

private:
	Interpolation PosInterp, DirInterp;
	bool hasDir;
};

void LoadMarks(string filename, vector<InterpData> &marks)
{
	ifstream input(filename);
	if (input.fail()) {
		cout << "\"" << filename << "\"" << "doesn't exist" << endl;
		return;
	}
	marks.clear();
	double t;
	Vector3 pos, dir;
	while (input >> t >> pos.x >> pos.y >> pos.z >> dir.x >> dir.y >> dir.z)
		marks.push_back(InterpData(t, pos, dir));
	input.close();
}

void SaveMarks(string filename, vector<InterpData> &marks)
{
	if (marks.size() == 0)
		return;
	double start = marks[0].Time;
	for (int i = 0; i < marks.size(); i++)
		marks[i].Time -= start;

	ofstream output(filename);
	output.clear();

	for (auto &m : marks) {
		auto pos = m.Position;
		auto dir = m.Direction;
		output << m.Time << "\t";
		output << pos.x << "\t" << pos.y << "\t" << pos.z << "\t";
		output << dir.x << "\t" << dir.y << "\t" << dir.z << endl;
	}
	output.close();
}