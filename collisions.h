#ifndef COLLISIONS_H
#define COLLISIONS_H

//======================== COLLISION WITH END OF THE MAP(CUBE) =================================

bool collisionOnX(glm::vec3 corner)
{
	//World
	if (corner.x <= -100 || corner.x >= 100) {
		return true;
	}
	return false;
}



bool collisionOnY(glm::vec3 corner)
{
	//World
	if (corner.y <= 0.2f) {
		return true;
	}
	return false;
}



bool collisionOnZ(glm::vec3 corner)
{
	//World
	if (corner.z <= -100 || corner.z >= 100) {
		return true;
	}
	return false;
}



glm::bvec3 collisionDetect(glm::mat4 futurePlaneM) {
	glm::mat3 futureRotation = glm::mat3(futurePlaneM);
	glm::vec3 futurePosition = glm::vec3(futurePlaneM[3][0], futurePlaneM[3][1], futurePlaneM[3][2]);
	glm::vec3 futureLeftUpperCorner = futurePosition + futureRotation * glm::vec3(-6, 1.6, 3);
	glm::vec3 futureLeftBottomCorner = futurePosition + futureRotation * glm::vec3(-6, 0.5, 3);
	glm::vec3 futureRightUpperCorner = futurePosition + futureRotation * glm::vec3(6, 1.6, 3);
	glm::vec3 futureRightBottomCorner = futurePosition + futureRotation * glm::vec3(6, 0.5, 3);
	glm::vec3 futureBottom = futurePosition + futureRotation * glm::vec3(0, 0.5, -3);

	glm::vec3 corners[5] = { futureLeftUpperCorner, futureRightBottomCorner ,futureLeftBottomCorner,futureRightUpperCorner, futureBottom };

	bool collisionX;
	bool collisionY;
	bool collisionZ;

	if (futurePosition.x < 93 && futurePosition.x > -93) collisionX = false;
	else {
		for (int i = 0; i < 5; i++) {
			collisionX = collisionOnX(corners[i]);
			if (collisionX == true) break;
		}
	}

	if (futurePosition.y < 93 && futurePosition.y > 8) collisionY = false;
	else {
		for (int i = 0; i < 5; i++) {
			collisionY = collisionOnY(corners[i]);
			if (collisionY == true) break;
		}
	}

	if (futurePosition.z < 93 && futurePosition.z > -93) collisionZ = false;
	else {
		for (int i = 0; i < 5; i++) {
			collisionZ = collisionOnZ(corners[i]);
			if (collisionZ == true) break;
		}
	}

	glm::bvec3 collision = glm::bvec3(collisionX, collisionY, collisionZ);

	return collision;
}

//========================= COLLISION WITH BUILDINGS =======================================

bool cornerCollisionWithBuilding(glm::vec3 corner)
{
	if (corner.y > 25.5) return false;
	bool x_check;
	bool y_check;
	bool z_check;

	int i;

	//kind of binary tree to optimalize 
	if (corner.x >= -50) {
		if (corner.z >= -50) {
			if (corner.z >= -27.5) {
				if (corner.x >= -27.5) i = 0; //building1
				else i = 1; //building2
			}
			else {
				if (corner.x >= -27.5) i = 2; //building3
				else i = 3; //building4
			}
		}
		else {
			if (corner.z >= -72.5) {
				if (corner.x >= -27.5) i = 4; //building5
				else i = 5;//building6
			}
			else {
				if (corner.x >= -27.5) i = 6;//building
				else i = 7; //building8
			}
		}
	}
	else {
		if (corner.z >= -50) {
			if (corner.z >= -27.5) {
				if (corner.x >= -72.5) i = 8; //building9
				else i = 9; //building10
			}
			else {
				if (corner.x >= -72.5) i = 10; //building11
				else i = 11; //building12
			}
		}
		else {
			if (corner.z >= -72.5) {
				if (corner.x >= -72.5) i = 12; //building13
				else i = 13;//building14
			}
			else {
				if (corner.x >= -72.5) i = 14;//building15
				else i = 15;//building16
			}
		}
	}


	y_check = (corner.y <= y_buildings[i]);
	if (!y_check) return false;
	x_check = (corner.x <= x_buildings[2 * i]) && (corner.x >= x_buildings[2 * i + 1]);
	if (!x_check) return false;
	z_check = (corner.z <= z_buildings[2 * i]) && (corner.z >= z_buildings[2 * i + 1]);
	if (!z_check) return false;
	else return true;


	return false;
}



bool CollisionWithBuilding(glm::mat4 planeM)
{
	glm::mat3 rotation = glm::mat3(planeM);
	glm::vec3 position = glm::vec3(planeM[3][0], planeM[3][1], planeM[3][2]);
	glm::vec3 leftUpperCorner = position + rotation * glm::vec3(-6, 1.6, 3);
	glm::vec3 leftBottomCorner = position + rotation * glm::vec3(-6, 0.5, 3);
	glm::vec3 rightUpperCorner = position + rotation * glm::vec3(6, 1.6, 3);
	glm::vec3 rightBottomCorner = position + rotation * glm::vec3(6, 0.5, 3);
	glm::vec3 middle = position + rotation * glm::vec3(0, 0.9, 3);
	glm::vec3 corners[5] = { leftUpperCorner, middle, rightBottomCorner,leftBottomCorner, rightUpperCorner };

	bool collision;

	for (int i = 0; i < 5; i++) {
		collision = cornerCollisionWithBuilding(corners[i]);
		if (collision) break;
	}

	return collision;
}
#endif
