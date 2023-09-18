/* -----------------------------------------------------------------------------
 * Copyright 2022 Massachusetts Institute of Technology.
 * All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Research was sponsored by the United States Air Force Research Laboratory and
 * the United States Air Force Artificial Intelligence Accelerator and was
 * accomplished under Cooperative Agreement Number FA8750-19-2-1000. The views
 * and conclusions contained in this document are those of the authors and should
 * not be interpreted as representing the official policies, either expressed or
 * implied, of the United States Air Force or the U.S. Government. The U.S.
 * Government is authorized to reproduce and distribute reprints for Government
 * purposes notwithstanding any copyright notation herein.
 * -------------------------------------------------------------------------- */
#pragma once
#include <geometry_msgs/Pose.h>
#include <hydra/reconstruction/reconstruction_module.h>
#include <pose_graph_tools/PoseGraph.h>
#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include <tf2_ros/transform_listener.h>

#include "hydra_ros/pipeline/ros_reconstruction_config.h"

namespace hydra {

class ImageReceiver;

class RosReconstruction : public ReconstructionModule {
 public:
  using PointcloudQueue = InputQueue<sensor_msgs::PointCloud2::ConstPtr>;

  RosReconstruction(const RosReconstructionConfig& config,
                    const ros::NodeHandle& nh,
                    const RobotPrefixConfig& prefix,
                    const OutputQueue::Ptr& output_queue = nullptr);

  virtual ~RosReconstruction();

  std::string printInfo() const override;

  void handlePointcloud(const sensor_msgs::PointCloud2::ConstPtr& msg);

  void handlePoseGraph(const pose_graph_tools::PoseGraph::ConstPtr& pose_graph);

  void handleAgentNodeMeasurements(const pose_graph_tools::PoseGraph::ConstPtr& msg);

 protected:
  bool checkPointcloudTimestamp(const ros::Time& curr_time);

  void pointcloudSpin();

 protected:
  const RosReconstructionConfig config_;
  ros::NodeHandle nh_;

  ros::Subscriber pcl_sub_;
  std::unique_ptr<ImageReceiver> image_receiver_;
  ros::Subscriber pose_graph_sub_;
  ros::Subscriber agent_node_meas_sub_;
  std::unique_ptr<tf2_ros::Buffer> buffer_;
  std::unique_ptr<tf2_ros::TransformListener> tf_listener_;

  PointcloudQueue pointcloud_queue_;
  std::unique_ptr<std::thread> pointcloud_thread_;
  std::unique_ptr<ros::Time> last_time_received_;

  std::mutex pose_graph_mutex_;
  std::list<pose_graph_tools::PoseGraph::ConstPtr> pose_graphs_;

  std::mutex agent_measurement_mutex_;
  pose_graph_tools::PoseGraph::ConstPtr agent_node_measurements_;

  ros::Publisher mesh_pub_;
  ros::Publisher pcl_pub_;
};

}  // namespace hydra
