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

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"

#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>


class Achievement : public SimObject
{
    typedef SimObject Parent;
    
public:
    
    // Constructor (currently does nothing)
    Achievement();
    
    // Called when an object is created in script or registered in C++
    bool onAdd();
    
    // Called when an object is deleted from the system
    void onRemove();
    
    // Exposes the Achievement member variables to TorqueScript
    static void initPersistFields();
    
    // Debug function used to print all achievement data to the console
    void printAchievement();
    
    // Helper function that sets a Achievement's member variables in C++
    void setAchievementFields(const char* identifier, const char* title, const char* achievedDescription, const char* unachievedDescription, int maxPoints, bool hidden);
    
    
    // The following member variables are the iT2D equivalent of what is found in Game Center
    // It is important to note that the actual properties in Game Center are read-only.
    
    // A unique string used to identify the achievement
    StringTableEntry mIdentifier;
    
    // A localized description of the completed achievement
    StringTableEntry mTitle;
    
    // A localized description of the completed achievement
    StringTableEntry mAchievedDescription;
    
    // 	A localized description of the achievement, to be used when the achievement has not been completed
    StringTableEntry mUnachievedDescription;
    
    // A Boolean value that states whether this achievement should be visible to players
    bool mHidden;
    
    // The number of points earned by completing this achievement
    int mMaximumPoints;

    DECLARE_CONOBJECT(Achievement);
};

#endif

// Main Game Center interface. This is what is responsible for all Game Center functionality
// This object is exposed to the rest of the engine via a static variable: gameCenterManager
// The declaration of this is found at the bottom of this header
@interface GameCenterManager : UIViewController <GKLeaderboardViewControllerDelegate, GKAchievementViewControllerDelegate>
{
    NSMutableDictionary* achievementCache;
    

    UIViewController *gameCenterViewController;
    
    BOOL isAuthenticated;
    BOOL gameCenterAvailable;
    int achievementCount;
}

@property (retain) NSMutableDictionary* achievementCache;
@property int achievementCount;

@property (nonatomic, retain) UIViewController* gameCenterViewController;

- (void) authenticateLocalUser;
- (void) registerForAuthenticationNotification: (NSError*) error;

- (void) reportScore: (int64_t) score forCategory: (NSString*) category;
- (void) reloadHighScoresForCategory: (NSString*) category;
- (void) showLeaderboard;
- (void) leaderboardViewControllerDidFinish:(GKLeaderboardViewController *)viewController;

- (void) submitAchievement: (NSString*) identifier percentComplete: (float) percentComplete;
- (void) resetAchievements;
- (void) showAchievements;
- (void) cacheAchievements;
- (void) achievementViewControllerDidFinish:(GKAchievementViewController *)viewController;

@end

// The global variable used when interfacing with TorqueScript
static GameCenterManager* gameCenterManager;

#endif //TORQUE_ALLOW_GAMEKIT