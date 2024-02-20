#include "Rootjob.h"
#include "SceneManager.h"

Rootjob::Rootjob()
	:GameObject(nullptr, "RootJob")
{
}

Rootjob::~Rootjob()
{
}

void Rootjob::Initialize()
{
	Instantiate<SceneManager>(this);
}

void Rootjob::Update()
{
}

void Rootjob::Draw()
{
}

void Rootjob::Release()
{
}
