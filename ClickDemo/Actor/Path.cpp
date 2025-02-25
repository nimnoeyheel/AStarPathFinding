#include "Path.h"

Path::Path(const Vector2& position)
	: DrawableActor("x") 
{
	this->position = position;
	color = Color::Red;
}