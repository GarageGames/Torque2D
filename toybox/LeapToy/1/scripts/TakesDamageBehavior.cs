//-----------------------------------------------------------------------------
// Torque
// Copyright GarageGames, LLC 2013
//-----------------------------------------------------------------------------

function LeapToy::createTakesDamageBehavior( %this )
{
    if (!isObject(TakesDamageBehavior))
    {
        %this.TakesDamageBehavior = new BehaviorTemplate(TakesDamageBehavior);
        
        %this.TakesDamageBehavior.friendlyName = "Takes Damage Advanced";
        %this.TakesDamageBehavior.behaviorType = "Game";
        %this.TakesDamageBehavior.description  = "Set the object to take damage from DealsDamage objects that collide with it";

        %this.TakesDamageBehavior.addBehaviorField(health, "The amount of health the object has", int, 10);
        %this.TakesDamageBehavior.addBehaviorField(lives, "The number of times the object can lose all its health", int, 3);
        %this.TakesDamageBehavior.addBehaviorField(respawnTime, "The time between death and respawn (seconds)", float, 2.0);
        %this.TakesDamageBehavior.addBehaviorField(invincibleTime, "The time after spawning before damage is applied (seconds)", float, 1.0);
        %this.TakesDamageBehavior.addBehaviorField(respawnEffect, "The particle effect to play on spawn", asset, "", ParticleAsset);
        %this.TakesDamageBehavior.addBehaviorField(explodeEffect, "The particle effect to play on death", asset, "", ParticleAsset);
        
        %this.add(%this.TakesDamageBehavior);
    }    
}

//-----------------------------------------------------------------------------

function TakesDamageBehavior::initialize(%this, %health, %lives, %respawnTime, %invincibleTime, %respawnEffect, %explodeEffect, %startFrame)
{
    %this.health = %health;
    %this.lives = %lives;
    %this.respawnTime = %respawnTime;
    %this.invincibleTime = %invincibleTime;
    %this.respawnEffect = %respawnEffect;
    %this.explodeEffect = %explodeEffect;
    %this.startFrame = %startFrame;
    %this.startHealth = %this.health;
}

//-----------------------------------------------------------------------------

function TakesDamageBehavior::onBehaviorAdd( %this )
{
    %this.startHealth = %this.health;    
    %this.owner.collisionCallback = true;
    %this.spawn();
}

//-----------------------------------------------------------------------------

function TakesDamageBehavior::takeDamage( %this, %amount, %assailant )
{
    if (%this.invincible)
        return;
   
    %this.health -= %amount;
    
    if (%this.health <= 0)
    {
        // Explode and kill the object
        %this.explode();
        %this.kill();
        return;
    }
}

//-----------------------------------------------------------------------------

function TakesDamageBehavior::kill( %this )
{
    %this.lives--;
    
    if (%this.lives <= 0)
    {
        %this.owner.safeDelete();
        return;
    }

    %this.invincible = true;
    %this.owner.visible = false;
    %this.owner.setCollisionSuppress(true);
    %this.owner.collisionActiveReceive = false;
    %this.schedule(%this.respawnTime * 1000, "spawn");
}

//-----------------------------------------------------------------------------

function TakesDamageBehavior::setVincible(%this)
{
    %this.invincible = false;
}

//-----------------------------------------------------------------------------

function TakesDamageBehavior::spawn( %this )
{
    %this.owner.collisionActiveReceive = true;
    %this.schedule(%this.invincibleTime * 1000, "setVincible");
    %this.health = %this.startHealth;
    %this.owner.visible = true;
    %this.owner.setCollisionSuppress(false);
    %this.owner.setImageFrame(%this.startFrame);
    
    if (%this.respawnEffect !$= "")
    {
        %particlePlayer = new ParticlePlayer();
        %particlePlayer.BodyType = static;
        %particlePlayer.Size = "10";
        %particlePlayer.SetPosition( %this.owner.getPosition() );
        %particlePlayer.SceneLayer = %this.owner.getSceneLayer();
        %particlePlayer.ParticleInterpolation = true;
        %particlePlayer.Particle = %this.respawnEffect;
        SandboxScene.add(%particlePlayer);
    }
}

//-----------------------------------------------------------------------------

function TakesDamageBehavior::explode( %this )
{
    if (%this.explodeEffect !$= "")
    {
        %particlePlayer = new ParticlePlayer();
        %particlePlayer.BodyType = static;
        %particlePlayer.SetPosition( %this.owner.getPosition() );
        %particlePlayer.SceneLayer = %this.owner.getSceneLayer();
        %particlePlayer.ParticleInterpolation = true;
        %particlePlayer.Particle = %this.explodeEffect;
        %particlePlayer.SizeScale = 0.8;
        SandboxScene.add(%particlePlayer);
    }
}