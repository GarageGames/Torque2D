//-----------------------------------------------------------------------------
// Torque
// Copyright GarageGames, LLC 2013
//-----------------------------------------------------------------------------

function LeapToy::createDealsDamageBehavior( %this )
{
    if (!isObject(DealsDamageBehavior))
    {
        %this.DealsDamageBehavior = new BehaviorTemplate(DealsDamageBehavior);

        %this.DealsDamageBehavior.friendlyName = "Deals Damage Advanced";
        %this.DealsDamageBehavior.behaviorType = "Game";
        %this.DealsDamageBehavior.description  = "Set the object to deal damage to TakesDamage objects it collides with";

        %this.DealsDamageBehavior.addBehaviorField(strength, "The amount of damage the object deals", int, 10);
        %this.DealsDamageBehavior.addBehaviorField(deleteOnHit, "Delete the object when it collides", bool, 1);
        %this.DealsDamageBehavior.addBehaviorField(explodeEffect, "The particle effect to play on collision", asset, "", ParticleAsset);

        %this.add(%this.DealsDamageBehavior);
    }    
}

//-----------------------------------------------------------------------------

function DealsDamageBehavior::initialize(%this, %strength, %deleteOnHit, %explodeEffect)
{
    %this.strength = %strength;
    %this.deleteOnHit = %deleteOnHit;
    %this.explodeEffect = %explodeEffect;
}

//-----------------------------------------------------------------------------

function DealsDamageBehavior::onBehaviorAdd(%this)
{
    %this.owner.collisionCallback = true;
    %this.owner.collisionActiveSend = true;
}

//-----------------------------------------------------------------------------

function DealsDamageBehavior::dealDamage(%this, %amount, %victim)
{
    %takesDamage = %victim.getBehavior("TakesDamageBehavior");

    if (!isObject(%takesDamage))
        return;
   
    %takesDamage.takeDamage(%amount, %this);
}

//-----------------------------------------------------------------------------

function DealsDamageBehavior::explode(%this, %position)
{
    if (%this.explodeEffect !$= "")
    {
        %particlePlayer = new ParticlePlayer();
        %particlePlayer.BodyType = static;
        %particlePlayer.SetPosition( %this.owner.getPosition() );
        %particlePlayer.SceneLayer = %this.owner.getSceneLayer();
        %particlePlayer.ParticleInterpolation = true;
        %particlePlayer.Particle = %this.explodeEffect;
        SandboxScene.add(%particlePlayer);
    }
}

//-----------------------------------------------------------------------------

function DealsDamageBehavior::onCollision(%this, %object, %collisionDetails)
{
    %this.dealDamage(%this.strength, %object);
   
    if (%this.deleteOnHit)
    {
        %this.explode(getWords(%contacts, 0, 1));
        %this.owner.safeDelete();
    }
}