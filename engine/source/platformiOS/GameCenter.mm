//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------
#ifdef TORQUE_ALLOW_GAMEKIT
#import "GameCenter.h"

#include "platformiOS.h"
#include "string/stringBuffer.h"
#include "sim/simBase.h"

extern iOSPlatState platState;

IMPLEMENT_CONOBJECT(Achievement);

// Blank constructor for Achievement class
Achievement::Achievement()
{
}

// Called when declared in script or registered
bool Achievement::onAdd()
{
    // Perform onAdd routine from parent class(es)
    if (!Parent::onAdd())
        return false;
    
    // Add to named AchievementSet
    // This set is available from anywhere in TorqueScript as AchievementSet
    Sim::getAchievementSet()->addObject(this);

    // Call onAdd in script!
    Con::executef(this, 2, "onAdd", Con::getIntArg(getId()));
    
    // Everything went well, return true
    return true;
}

// Called when deleted from script
void Achievement::onRemove()
{
    // We call this on this objects namespace so we unlink them after
    // Call onRemove in script!
    Con::executef(this, 2, "onRemove", Con::getIntArg(getId()));
    
    Parent::onRemove();
}

// Expose all the Achievement fields to TorqueScript
void Achievement::initPersistFields()
{
    // The following member variables are the iT2D equivalent of what is found in Game Center
    // It is important to note that the actual properties in Game Center are read-only
    
    addField("identifier", TypeString, Offset(mIdentifier, Achievement), "A unique string used to identify the achievement");
    addField("title", TypeString, Offset(mTitle, Achievement), "Achievement Title");
    addField("achievedDescription", TypeString, Offset(mAchievedDescription, Achievement), "A localized description of the completed achievement");
    addField("unachievedDescription", TypeString, Offset(mUnachievedDescription, Achievement), "A localized description of the achievement, to be used when the achievement has not been completed");
    addField("hidden", TypeBool, Offset(mHidden, Achievement), "A Boolean value that states whether this achievement should be visible to players");
    addField("maximumPoints", TypeS32, Offset(mMaximumPoints, Achievement), "The number of points earned by completing this achievement");
}

// Helper function used to set all the achievement fields
// This is only useful when creating a Achievement in C++
// Currently the only example of this is used in cacheAchievements,
// converting a GKAchievementDescription into a Achievement
void Achievement::setAchievementFields(const char* identifier, const char* title, const char* achievedDescription, const char* unachievedDescription, int maxPoints, bool hidden)
{
    mIdentifier = StringTable->insert(identifier);
    mTitle = StringTable->insert(title);
    mAchievedDescription = StringTable->insert(achievedDescription);
    mUnachievedDescription = StringTable->insert(unachievedDescription);
    mMaximumPoints = maxPoints;
    mHidden = hidden;
}

// Print all the Achievement fields to the console
// This is only useful for debugging purposes
void Achievement::printAchievement()
{
    Con::printf("Achievement identifier: %s", mIdentifier);
    Con::printf("Achievement title: %s", mTitle);
    Con::printf("Achievement description: %s", mAchievedDescription);
    Con::printf("Achievement un-description: %s", mUnachievedDescription);
    Con::printf("Achievement maximum points: %i", mMaximumPoints);
    Con::printf("Achievement hidden: %i", mHidden);
}

// Exposes the Achievement::printAchievement() function to TorqueScript
// only useful for debugging purposes
ConsoleMethod(Achievement, printAchievement, void, 2, 2, "() Prints out the properties of an achievement")
{
    object->printAchievement();
}

// Utility function that will determine whether the current iOS support Game Center
BOOL isGameCenterAvailable()
{
    // Build a class from a string, using a Game Center specific class: GKLocalPlayer
    // http://developer.apple.com/library/ios/#documentation/GameKit/Reference/GKLocalPlayer_Ref/Reference/Reference.html
    Class gcClass = (NSClassFromString(@"GKLocalPlayer"));
    
    // This build of iT2D is shooting for a minimum of iOS 4.1
    NSString *reqSysVer = @"4.1";
    NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
    
    BOOL osVersionSupported = ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending);
    
    return (gcClass && osVersionSupported);
}

// Primary Game Center Objective-C starts here
@implementation GameCenterManager

@synthesize achievementCache;
@synthesize gameCenterViewController;
@synthesize achievementCount;


// Main initialization function for GameCenterManager
// This is exposed to TorqueScript via the initGameCenter() function
- (id) init
{
    self = [super init];
    
    if(self != NULL)
    {
        gameCenterAvailable = NO;
        isAuthenticated = NO;
        achievementCount = 0;
        achievementCache = NULL;
        gameCenterViewController = [[UIViewController alloc] init];
        gameCenterViewController.view.hidden = true;

    }
    
    return self;
}

// Main cleanup function for GameCenterManager
// This is exposed to TorqueScript via the closeGameCenter() function
- (void) dealloc
{
    // TODO - Call this from script
    gameCenterAvailable = NO;
    isAuthenticated = NO;
    
    self.achievementCache = NULL;
    
    if(gameCenterViewController != NULL)
        [gameCenterViewController release];
    
    [super dealloc];
}

// This is the second part of initializing Game Center. The init function simply allocates memory.
// This function actually connects and authenticates the current user
// Exposed to TorqueScript via the initGameCenter() function
- (void) authenticateLocalUser
{
    // Assume the player is not currently authenticated
    isAuthenticated = NO;
    
    // Check to see if this iOS supports Game Center
	gameCenterAvailable = isGameCenterAvailable();
	
    // Game Center not supported. Abort and print the error
	if(!gameCenterAvailable)
    {
        Con::printf("Failed to authenticate player. This iOS does not support Game Center");
		return;
	}
	
    // The iOS supports Game Center, so attempt to authenticate
    [[GKLocalPlayer localPlayer] authenticateWithCompletionHandler:^(NSError *error)
    {
        // Something went wrong with the authentication
        // Find out what the error is and print it to the console
		if (error != nil)
        {
            const char* errorMessage = [[error localizedDescription] UTF8String];
            Con::printf("Failed to authenticate player. %s", errorMessage);
            
            // Let the scripts know. 0 means it failed
            Con::executef(2, "gameCenterAuthenticationChanged", "0");
        }
        else
        {
            // Successful authentication occurred
			isAuthenticated = YES;
            
            // Start listening for changes in authentication
			[self registerForAuthenticationNotification:error];
			
            // Cache the achievements
			[self cacheAchievements];
            
			// Let the scripts know. 1 means it succeeded
			Con::executef(2, "gameCenterAuthenticationChanged", "1");
		}
	}];
}

// Helper function. This watches for any changes to the authentication for the Local Player 
- (void)registerForAuthenticationNotification: (NSError*) error
{
    if(error == NULL)
	{
        NSNotificationCenter* nc = [NSNotificationCenter defaultCenter];
        
        [nc addObserver:self selector:@selector(authenticationChanged) name:GKPlayerAuthenticationDidChangeNotificationName object:nil];

		Con::printf("Game Center Authentication has changed");
	}
}

// Signaled by registerForAuthenticationNotification
// This function will find out how the authentication changed
- (void) authenticationChanged
{
    gameCenterAvailable = isGameCenterAvailable();
        
    if(!gameCenterAvailable)
    {
        Con::printf("Authentication failed: Game Center is not initialized or supported on this iOS.");
        return;
    }
    
    // Check to see if player is still authenticated
    if([GKLocalPlayer localPlayer].authenticated == YES)
    {
        isAuthenticated = YES;
        
        Con::printf("Authentication changed successfully");
        Con::executef(2, "gameCenterAuthenticationChanged", "1");
    }
    else
    {
        Con::printf("Authentication failed for Local Player");
        Con::executef(2, "gameCenterAuthenticationChanged", "0");
    }
}

// Reload all the scores for a specific leaderboard (determined by category)
// Exposed to TorqueScript via reloadHighScores(%category);
- (void) reloadHighScoresForCategory:(NSString *)category
{
    GKLeaderboard* leaderBoard = [[[GKLeaderboard alloc] init] autorelease];
    
    leaderBoard.playerScope = GKLeaderboardPlayerScopeGlobal;
    leaderBoard.category = category;
    leaderBoard.timeScope = GKLeaderboardTimeScopeAllTime;
    leaderBoard.range = NSMakeRange(1, 15);
    
    [leaderBoard loadScoresWithCompletionHandler:^(NSArray *scores, NSError *error) 
    {
        if(error != nil)
        {
            const char* errorMessage = [[error localizedDescription] UTF8String];
            Con::printf("Failed to reload highscores: %s", errorMessage);
            
            // Perform custom error handling here
        }
        else
        {
            const char* board = [category UTF8String];
            Con::printf("Highscores successfully reloaded for %s", board);
            
            // Perform success code here
        }
    }];
}

// Responsible for reporting a new score to a leaderboard (determined by category)
// Exposed to TorqueScript via reportScore(%score, %category);
- (void) reportScore:(int64_t)score forCategory:(NSString *)category
{
    GKScore* scoreReporter = [[[GKScore alloc] initWithCategory:category] autorelease];
    
    scoreReporter.value = score;
    
    [scoreReporter reportScoreWithCompletionHandler:^(NSError *error) 
    {
        if(error != nil)
        {
            const char* errorMessage = [[error localizedDescription] UTF8String];
            Con::printf("Failed to submit score. %s", errorMessage);
        }
        else
        {
            [self reloadHighScoresForCategory:category];
            const char* board = [category UTF8String];
            Con::printf("Score successfully submitted to %s", board);
            
            // Perform custom success code here (saving, notification, etc)
        }
}];
}

// Displays the leaderboard using Game Center's interface
- (void) showLeaderboard
{
    // Check to see if local player is signed in
    // If not, error out and exit function
    if([GKLocalPlayer localPlayer].authenticated == NO)
    {
        Con::printf("Failed to show leaderboard: Local Player not authenticated");
        return;
    }
            
    GKLeaderboardViewController *leaderBoardController = [[GKLeaderboardViewController alloc] init];
        
    if(leaderBoardController != nil)
    {
        leaderBoardController.leaderboardDelegate = self;
            
        UIWindow* window = [UIApplication sharedApplication].keyWindow;

        [window addSubview: gameCenterViewController.view];

        [gameCenterViewController presentModalViewController:leaderBoardController animated:YES];
    }
}

// Called when the user finishes viewing the Leader Board view
- (void) leaderboardViewControllerDidFinish:(GKLeaderboardViewController *)viewController
{
    [gameCenterViewController dismissModalViewControllerAnimated:YES];
    [viewController.view removeFromSuperview];
    [viewController release];
}

// Responsible for submitting achievement progress
// identifier is the unique string for the achievement
// percentComplete determines how far the user has progressed to unlocking the achievement
// Exposed to TorqueScript via submitAchievement(%identifier, %percentComplete);
- (void) submitAchievement:(NSString *)identifier percentComplete:(float)percentComplete
{

    GKAchievement* achievement = [self.achievementCache objectForKey:identifier];
        
    if(achievement != NULL)
    {
        if((achievement.percentComplete >= 100.0) || (achievement.percentComplete >= percentComplete))
        {
            achievement = NULL;
            achievement.percentComplete = percentComplete;
        }
    }
    else
    {
        achievement = [[[GKAchievement alloc] initWithIdentifier:identifier] autorelease];
        achievement.percentComplete = percentComplete;
            
        [self.achievementCache setObject:achievement forKey:achievement.identifier];
    }
        
    if(achievement != NULL)
    {
        [achievement reportAchievementWithCompletionHandler:^(NSError *error)
        {
			if (error != nil)
            {
				const char* errorMessage = [[error localizedDescription] UTF8String];
                Con::printf("Failed to submit achievement: %s", errorMessage);
                
                // Perform custom error handling here
			}
            else
            {
                const char* achievementID = [identifier UTF8String];
                Con::printf("Achievement %s successfully submitted", achievementID);
                
                // Perform custom success code here (saving, notification, etc)
            }
		}];
    }
}
// Resets all achievement progress for a user
// Exposed to TorqueScript via resetAchievements();
- (void) resetAchievements
{
    self.achievementCache = NULL;
    [GKAchievement resetAchievementsWithCompletionHandler:^(NSError *error) 
    {
        if(error != nil)
        {
            const char* errorMessage = [[error localizedDescription] UTF8String];
            Con::printf("Failed to submit achievement: %s", errorMessage);
            
            // Perform custom error handling here
        }
        else
        {
            Con::printf("Achievements reset");
            
            // Perform custom success handling here
        }
    }];
}

// Displays the achievements using Game Center's view
- (void)showAchievements
{	
    // Check to see if local player is signed in
    // If not, error out and exit function
	if([GKLocalPlayer localPlayer].authenticated == NO)
    {
        Con::printf("Failed to show achievements: Local Player not authenticated");
        return;
    }
	
    GKAchievementViewController *achievements = [[GKAchievementViewController alloc] init];

    if (achievements != nil)
    {
        achievements.achievementDelegate = self;
		
		UIWindow* window = [UIApplication sharedApplication].keyWindow;
		[window addSubview: gameCenterViewController.view];
        
        [gameCenterViewController presentModalViewController: achievements animated: YES];
    }	
}

// Called when the user finishes viewing the achievements view
- (void)achievementViewControllerDidFinish:(GKAchievementViewController *)viewController
{
    [gameCenterViewController dismissModalViewControllerAnimated:YES];
	[viewController.view removeFromSuperview];
	[viewController release];
}

// This extremely important function is used for grabbing all the achievements this app supports
// and exposing it to TorqueScript. This will create new t2DAchievements and populate them
// with data accessed from iTunes Connect. This will only work if you have an iTunes Connect
// account setup, along with a registered app with achievements.
//
// For more information on iTunes Connect, see the following two links:
// iT2D Official Documentation: http://docs.garagegames.com/it2d/official
// iTunes Connect Doc: https://itunesconnect.apple.com/docs/iTunesConnect_DeveloperGuide.pdf
- (void)cacheAchievements
{
    if(self.achievementCache == NULL)
    {
        [GKAchievement loadAchievementsWithCompletionHandler:^(NSArray *achievements, NSError *error) 
         {
             if(error != nil)
             {
                 const char* errorMessage = [[error localizedDescription] UTF8String];
                 Con::printf("Failed to cache achievements. %s", errorMessage);
                 
                 // Perform custom caching code here (loading from file, retrying, etc)
             }
             else
             {
                 NSMutableDictionary* tempCache = [NSMutableDictionary dictionaryWithCapacity: [achievements count]];
                 for(GKAchievement* achievement in achievements)
                 {
                     Con::printf("Achievement progres: %f", achievement.percentComplete);
                     [tempCache setObject: achievement forKey: achievement.identifier];
                 }
                 self.achievementCache = tempCache;
             }
         }];
    }
    [GKAchievementDescription loadAchievementDescriptionsWithCompletionHandler:
     ^(NSArray *descriptions, NSError *error) 
    {
        // Something went wrong when trying to access the achievement list
        // The error should contain some information, so print the localized string
        if (error != nil)
        {
            const char* errorMessage = [[error localizedDescription] UTF8String];
            Con::printf("Failed to cache achievements. %s", errorMessage);
        }
        else
        {
            // If there are no achievements, the array will be nil
            // Otherwise, start iteration through them
            if (descriptions != nil)
            {
                // For each individual achievement in the array
                for(GKAchievementDescription* description in descriptions)
                {
                    // Convert the GKAchievementDescription fields to iT2D compatible versions
                    const char* title = [description.title UTF8String];
                    const char* identifier = [description.identifier UTF8String];
                    const char* achievedDescription = [description.achievedDescription UTF8String];
                    const char* unachieveDescription = [description.unachievedDescription UTF8String];
                    int maxPoints = description.maximumPoints;
                    bool hidden = description.hidden;
                    
                    // First, check to see if this is a duplicate or not
                    // If it is, skip adding this
                    if(Sim::getAchievementSet()->findObject(identifier))
                    {
                        // Do not allocate memory or add this achievement
                        // it already exists in our AchievementSet
                        continue;
                    }
                    
                    // Create the new Achievement
                    Achievement* ach = new Achievement();
                                        
                    // Plug in the properties to the new Achievement
                    ach->setAchievementFields(identifier, title, achievedDescription, unachieveDescription, maxPoints, hidden);
                    
                    // Register this Achievement so it can be accessed from TorqueScript
                    // and properly cleaned up during shut down
                    ach->registerObject(identifier);
                }
                
            }
            // Store the number of achievements in GameCenterManager, just in case
            achievementCount = [descriptions count];
            
            Con::printf("Achievements cached");
        }
     }];
}

@end


// The rest of the code consists of exposing the GameCenterManager to TorqueScript
// via global ConsoleFunctions

ConsoleFunction(isGameCenterAvailable, bool, 1, 1, "() Used to determine if current iOS is capable of using Game Center\n"
                                                   "@return True if Game Center is supported, false otherwise")
{
    return isGameCenterAvailable();
}

ConsoleFunction(initGameCenter, bool, 1, 1, "() Initialize Game Center and authenticate the player\n"
                                            "@return True if Game Center was successfully initialized, false if there was an error")
{
    if(isGameCenterAvailable())
    {
        gameCenterManager = [[GameCenterManager alloc] init];
        [gameCenterManager authenticateLocalUser];
    }
}

ConsoleFunction(closeGameCenter, void, 1, 1, "() This will cleanup and close Game Center. Should only be called when the game is closed")
{
    [gameCenterManager release];
}

ConsoleFunction(showLeaderboard, void, 1, 1, "() Display the leaderboards for this app using Game Center's view")
{
    if(isGameCenterAvailable())
    {
        [gameCenterManager showLeaderboard];
    }
    else
    {
        Con::printf("Failed to show leaderboard: Game Center is not supported");
    }
    
}

ConsoleFunction(showAchievements, void, 1, 1, "() Display the achievements for this app using Game Center's view")
{
    if(isGameCenterAvailable())
    {
        [gameCenterManager showAchievements];
    }
    else
    {
        Con::printf("Failed to show achievements: Game Center is not supported");
    }
}

ConsoleFunction(getAchievementCount, int, 1, 1, "() Gets the number of achievements this app supports from GameCenterManager\n"
                                                "@return Number of achievements obtained by connecting to iTunes Connect")
{
    return [gameCenterManager achievementCount];
}

ConsoleFunction(cacheAchievements, void, 1, 1, "() Forces achievement cacheing and populates the AchievementSet")
{
    [gameCenterManager cacheAchievements];
}

ConsoleFunction(submitScore, bool, 3, 3, "(int score, string category) Submit a score to a leaderboard, defined by category\n"
                                         "@param score Numerical value of the score being reported\n"
                                         "@param category String containing the unique category of the leaderboard\n"
                                         "@return True if the score was successfully submitted, false if there was an error")
{
    // Convert the identifier from a char* to a NSString
    NSString* category = [[[NSString alloc] initWithUTF8String:argv[2]] autorelease];
    
    // Convert score from char* to an int
    int score = dAtoi(argv[1]);
    
    // Call the GameCenterManager method for reporting a score
    [gameCenterManager reportScore:score forCategory:category];
}

ConsoleFunction(reportAchievement, bool, 3, 3, "(string identifier, float percentComplete) Reports achievement progress to Game Center\n"
                "@param identifier A unique string used to identify the achievement\n"
                "@param percentComplete Floating point value representing the progress of the achievement\n"
                "@return True if the achievement succesfully registered, false if there was an error")
{
    // Convert the identifier from a char* to a NSString
    NSString* identifier = [[[NSString alloc] initWithUTF8String:argv[1]] autorelease];
    
    // Convert the percentComplete from a char* to a float
    float percentComplete = dAtof(argv[2]);
    
    // Call the GameCenterManager method for reporting an achievement
    [gameCenterManager submitAchievement:identifier percentComplete:percentComplete];
}

ConsoleFunction(reloadHighScores, void, 2, 2, "(string category) Reload the highscore for a leaderboard, specified by category")
{
    NSString* category = [[[NSString alloc] initWithUTF8String:argv[1]] autorelease];
    
    [gameCenterManager reloadHighScoresForCategory:category];
}
ConsoleFunction(resetAchievements, bool, 1, 1, "() Reset the achievement progress for the authenticated player. This cannot be undone\n"
                                               "@return True if the achievements were successfully cleared, false if there was an error")
{
    [gameCenterManager resetAchievements];
}
#endif //TORQUE_ALLOW_GAMEKIT