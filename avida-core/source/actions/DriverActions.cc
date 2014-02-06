/*
 *  DriverActions.cc
 *  Avida
 *
 *  Created by David Bryson on 7/19/06.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "DriverActions.h"

#include "avida/core/Feedback.h"
#include "avida/core/UniverseDriver.h"

#include "cAction.h"
#include "cActionLibrary.h"
#include "cStats.h"
#include "cWorld.h"
#include "cPopulation.h"

#include <ctime>

using namespace Avida;


class cActionExit : public cAction
{
public:
  cActionExit(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  static const cString GetDescription() { return "No Arguments"; }
  void Process(cAvidaContext&) { m_world->GetDriver().Finish(); }
};

class cActionPause : public cAction
{
public:
  cActionPause(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  static const cString GetDescription() { return "No Arguments"; }
  void Process(cAvidaContext&) { m_world->GetDriver().Pause(); }
};




/*! Exit Avida when the average generation is greater than or equal to a
threshold value.

*/
class cActionExitAveGeneration : public cAction {
public:
  /*! Constructor; parse out the targeted generation.
  */
  cActionExitAveGeneration(cWorld* world, const cString& args, Feedback&) : cAction(world, args) {
    cString largs(args);
    if(largs.GetSize()) {
      m_tgt_gen = largs.PopWord().AsDouble();
    } else {
      // error; no default value for targeted generation.
      m_world->GetDriver().Feedback().Error("ExitAveGeneration event requires generation.");
      m_world->GetDriver().Abort(Avida::INVALID_CONFIG);
    }
  }

  static const cString GetDescription() { return "Arguments: <double generation>"; }

  void Process(cAvidaContext&) {
      if(m_world->GetStats().GetGeneration() > m_tgt_gen) {
        m_world->GetDriver().Finish();
      }
  }
  
protected:
  double m_tgt_gen; //!< Target generation above which Avida should exit.
};


/*! Exit Avida when the elapsed wallclock time has exceeded a threshold number
of seconds, beginning from the construction of this object.
*/
class cActionExitElapsedTime : public cAction {
public:
  /*! Constructor; parse out the threshold time.
  */
  cActionExitElapsedTime(cWorld* world, const cString& args, Feedback&) : cAction(world, args) {
    cString largs(args);
    if(largs.GetSize()) {
      m_time = largs.PopWord().AsInt();
    } else {
      // error; no default value for elapsed time.
      m_world->GetDriver().Feedback().Error("ExitElapsedTime event requires elapsed time.");
      m_world->GetDriver().Abort(Avida::INVALID_CONFIG);
    }
    
    // When did we start?
    m_then = time(0);
  }
  
  static const cString GetDescription() { return "Arguments: <int elapsed time [seconds]>"; }
  
  /*! Check to see if we should exit Avida based on the elapsed time since construction
  of this object.  This method is called based on the events file.
  */
  void Process(cAvidaContext&) {
    if((time(0) - m_then) >= m_time) {
      m_world->GetDriver().Finish();
    }
  }
  
protected:
  time_t m_time; //!< Number of seconds after which Avida should exit.
  time_t m_then; //!< Time at which this object was constructed (the 'start' of Avida).
};




void RegisterDriverActions(cActionLibrary* action_lib)
{
  action_lib->Register<cActionExit>("Exit");
  action_lib->Register<cActionExitAveGeneration>("ExitAveGeneration");
  action_lib->Register<cActionExitElapsedTime>("ExitElapsedTime");
  action_lib->Register<cActionPause>("Pause");
}