/*
 * Copyright (C) 2020 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

// Usage:
//   1. You need to generate the list of overlapping points:
//     ign launch visibility.ign worldName:=simple_cave_02 2> log
//
//   2. Filter the log and leave just the "<x> <y> <z>" per line.
//     cat log | cut -d" " -f1,2,3 > simple_cave_02_overlaps.txt"
//
//   3. Load the world:
//     ign launch -v 4 cave_circuit.ign worldName:=simple_cave_02 robotName1:=X1 robotConfig1:=X1_SENSOR_CONFIG_8
//
//   4. Run this overlap visualizer:
//     ./install/bin/show_bb_overlaps simple_cave_02_overlaps.txt

#include <iostream>
#include <string>
#include <vector>
#include <ignition/transport.hh>
#include <ignition/math.hh>
#include <ignition/msgs.hh>
#include <ignition/common/Time.hh>

/////////////////////////////////////////////////
int main(int _argc, char **_argv)
{
  if (_argc != 2)
  {
    std::cerr << "Usage show_bb_overlaps <file>" << std::endl << std::endl;
    std::cerr << "Example: ./show_bb_overlaps points.txt" << std::endl;
    return -1;
  }

  std::ifstream in;
  in.open(_argv[1]);
  if (!in.is_open())
  {
    std::cerr << "Unable to find file [" << _argv[1] << "]" << std::endl;
    return -1;
  }

  std::vector<ignition::math::Vector3d> points;
  int counter = 0;

  std::string line;
  while (std::getline(in, line))
  {
    std::istringstream iss(line);
    double x, y, z;
    if (!(iss >> x >> y >> z))
    {
      // error
      break;
    }

    // Add a new point.
    points.push_back({x, y, z});
    ++counter;
  }

  std::cout << counter << " points loaded" << std::endl;

  ignition::transport::Node node;

  // Create the marker message
  ignition::msgs::Boolean res;
  bool result;
  unsigned int timeout = 5000;

  ignition::msgs::Marker markerMsg;
  markerMsg.set_id(5);
  markerMsg.set_action(ignition::msgs::Marker::ADD_MODIFY);
  markerMsg.set_type(ignition::msgs::Marker::POINTS);
  markerMsg.mutable_material()->mutable_ambient()->set_r(1);
  markerMsg.mutable_material()->mutable_ambient()->set_g(0);
  markerMsg.mutable_material()->mutable_ambient()->set_b(0);
  markerMsg.mutable_material()->mutable_diffuse()->set_r(1);
  markerMsg.mutable_material()->mutable_diffuse()->set_g(0);
  markerMsg.mutable_material()->mutable_diffuse()->set_b(0);
  markerMsg.mutable_material()->mutable_emissive()->set_r(1);
  markerMsg.mutable_material()->mutable_emissive()->set_g(0);
  markerMsg.mutable_material()->mutable_emissive()->set_b(0);
  markerMsg.clear_point();

  for (auto p : points)
  {
    ignition::msgs::Set(markerMsg.add_point(),
      ignition::math::Vector3d(p.X(), p.Y(), p.Z()));
  }

  node.Request("/marker", markerMsg);
  std::cout << "Points published" << std::endl;
}
