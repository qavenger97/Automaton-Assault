#include "pch.h"

#include "TechTest.h"

#include "Core\Entity.h"
#include "Behavior\IBehavior.h"
#include "Component\BehaviorTree.h"
#include "Component\ParticleEffect.h"
#include "Behavior\Sequence.h"
#include "Behavior\RepeatUntilFail.h"
#include "MoveInACircle.h"
#include "Renderer\RenderComponent.h"
#include "MoveInACircle.h"

void TechTest::BuildEntityTest()
{
/*	// Adding from "load" string
	Hourglass::BehaviorTree* bt = (Hourglass::BehaviorTree*)Hourglass::IComponent::Create( "BehaviorTree" );
	Hourglass::MoveInACircle* moveInACircle = (Hourglass::MoveInACircle*)Hourglass::MoveInACircle::Create();
	moveInACircle->SetTest( 0 );
	Hourglass::MoveInACircle* moveInACircle2 = (Hourglass::MoveInACircle*)Hourglass::MoveInACircle::Create();
	moveInACircle2->SetTest( 1 );
	Hourglass::RepeatUntilFail* root = (Hourglass::RepeatUntilFail*)Hourglass::RepeatUntilFail::Create();
	Hourglass::Sequence* seq = (Hourglass::Sequence*)Hourglass::Sequence::Create();
	seq->AddBehavior( moveInACircle );
	seq->AddBehavior( moveInACircle2 );
	root->SetBehavior( seq );
	bt->SetRoot( root );
	bt->SetEnabled( true );
	entity.AttachComponent( bt );

	Hourglass::PrimitiveRenderer* pr = (Hourglass::PrimitiveRenderer*)Hourglass::IComponent::Create( "PrimitiveRenderer" );
	pr->CreateColoredBox( Vec4( 0.97f, 0.69f, 0.58f, 1.0f ) );
	pr->SetEnabled( true );
	entity.AttachComponent( pr );

	entity.Init();

	// Active when should be?
	assert( entity.IsActive() );

	// When I get component, is it there when it is supposed to be?
	Hourglass::BehaviorTree treeToGetID;
	int id = treeToGetID.GetTypeID();

	Hourglass::IComponent* retrievedComponent = entity.GetComponent( treeToGetID.GetTypeID() );
	assert( retrievedComponent != nullptr );

	// Same entity?
	assert( &entity == retrievedComponent->GetEntity() );

	/*
	// Inactive and disabled after shutdown?
	entity.Shutdown();
	assert( !retrievedComponent->IsAlive() && !retrievedComponent->IsEnabled() );
	
	// Pointers null after detach?
	entity.DetachComponents();
	assert( !retrievedComponent->GetEntity() );
	assert( entity.GetComponent( id ) == nullptr );
	

	/// Actions
	Hourglass::Sequence* s = new Hourglass::Sequence();
	(Hourglass::Sequence*)Hourglass::IBehavior::Create( "Sequence" );
	*/
	
	
	
}