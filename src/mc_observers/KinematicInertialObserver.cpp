/*
 * Copyright 2015-2019 CNRS-UM LIRMM, CNRS-AIST JRL
 */

#include <mc_observers/KinematicInertialObserver.h>
#include <mc_observers/ObserverMacros.h>

#include <mc_control/MCController.h>

#include <mc_rtc/gui/Arrow.h>

namespace mc_observers
{

void KinematicInertialObserver::configure(const mc_control::MCController & ctl, const mc_rtc::Configuration & config)
{
  KinematicInertialPoseObserver::configure(ctl, config);
  config("showVelocity", showVelocity_);
  double cutoff = config("cutoff", 2 * ctl.timeStep);
  velFilter_.cutoffPeriod(cutoff);
  desc_ = name_ + " (cutoff=" + std::to_string(velFilter_.cutoffPeriod()) + ")";
}

void KinematicInertialObserver::reset(const mc_control::MCController & ctl)
{
  reset(ctl, ctl.realRobot(robot_).velW());
}

void KinematicInertialObserver::reset(const mc_control::MCController & ctl, const sva::MotionVecd & velW)
{
  KinematicInertialPoseObserver::reset(ctl);
  posWPrev_ = KinematicInertialPoseObserver::posW();
  velW_ = velW;
  velFilter_.reset(velW);
}

bool KinematicInertialObserver::run(const mc_control::MCController & ctl)
{
  bool res = KinematicInertialPoseObserver::run(ctl);
  const sva::PTransformd posW = KinematicInertialPoseObserver::posW();
  sva::MotionVecd errVel = sva::transformError(posWPrev_, posW) / ctl.timeStep;
  velFilter_.update(errVel);
  velW_ = velFilter_.eval();
  posWPrev_ = posW;
  return res;
}

void KinematicInertialObserver::updateRobots(mc_control::MCController & ctl)
{
  KinematicInertialPoseObserver::updateRobots(ctl);
  ctl.realRobot(robot_).velW(velW_);
}

const sva::MotionVecd & KinematicInertialObserver::velW() const
{
  return velW_;
}

void KinematicInertialObserver::addToLogger(mc_control::MCController & ctl, const std::string & category)
{
  KinematicInertialPoseObserver::addToLogger(ctl, category);
  auto & logger = ctl.logger();
  logger.addLogEntry(category + "_velW", [this]() -> const sva::MotionVecd & { return velW_; });
  logger.addLogEntry(category + "_filter_cutoffPeriod", [this]() { return velFilter_.cutoffPeriod(); });
}

void KinematicInertialObserver::removeFromLogger(mc_control::MCController & ctl, const std::string & category)
{
  KinematicInertialPoseObserver::removeFromLogger(ctl, category);
  auto & logger = ctl.logger();
  logger.removeLogEntry(category + "_velW");
  logger.removeLogEntry(category + "_filter_cutoffPeriod");
}

void KinematicInertialObserver::addToGUI(mc_control::MCController & ctl, const std::vector<std::string> & category)
{
  KinematicInertialPoseObserver::addToGUI(ctl, category);
  auto showHideVel = [this, category, &ctl]() {
    std::string name = "Velocity";
    auto cat = category;
    cat.push_back("Markers");
    ctl.gui()->removeElement(cat, name);
    if(showVelocity_)
    {
      ctl.gui()->addElement(
          cat, mc_rtc::gui::Arrow(name, [this]() -> const Eigen::Vector3d & { return posW().translation(); },
                                  [this]() -> Eigen::Vector3d {
                                    const Eigen::Vector3d p = posW().translation();
                                    Eigen::Vector3d end = p + velW().linear();
                                    return end;
                                  }));
    }
  };

  ctl.gui()->addElement(category, mc_rtc::gui::Checkbox("Show velocity", [this]() { return showVelocity_; },
                                                        [this, showHideVel]() {
                                                          showVelocity_ = !showVelocity_;
                                                          showHideVel();
                                                        }));

  ctl.gui()->addElement(
      category,
      mc_rtc::gui::NumberInput(
          "Cutoff Period", [this]() -> double { return velFilter_.cutoffPeriod(); },
          [this, &ctl](double cutoff) {
            if(cutoff < 2 * ctl.timeStep)
            {
              mc_rtc::log::warning(
                  "[{}] cutoff period must be at least twice the timestep (>={}), keeping the current value ({})",
                  name(), 2 * ctl.timeStep, velFilter_.cutoffPeriod());
              return;
            }
            velFilter_.cutoffPeriod(cutoff);
          }));

  showHideVel();
}

} // namespace mc_observers

EXPORT_OBSERVER_MODULE("KinematicInertial", mc_observers::KinematicInertialObserver)
