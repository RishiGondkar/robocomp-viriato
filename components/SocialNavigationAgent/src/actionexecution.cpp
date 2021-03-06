/*
 * Copyright 2017 <copyright holder> <email>
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

#include "actionexecution.h"

ActionExecution::ActionExecution()
{

  	worldModel = AGMModel::SPtr(new AGMModel());
	worldModel->name = "worldModel";
	innerModel = std::make_shared<InnerModel>(InnerModel());
	haveTarget = false;	
}

ActionExecution::~ActionExecution() 
{
  
}

void ActionExecution::Update(std::string a,ParameterMap prs)
{
	static bool first=true;

	if (first)
	{
		qLog::getInstance()->setProxy("both", logger_proxy);
		rDebug2(("ACTION EXECUTION started"));
		first = false;
	}

	if (worldModel->getIdentifierByType("robot") < 0)
	{
		try
		{
			RoboCompAGMWorldModel::World w = agmexecutive_proxy->getModel();
			structuralChange(w);
		}
		catch(...)
		{
			printf("The executive is probably not running, waiting for first AGM model publication...");
		}
	}
	
	action = a;
	params = prs;
	actionExecution();

}


void ActionExecution::actionExecution()
{
//	QMutexLocker locker(mx);
	qDebug() << "---------------------------------------------------";
	qDebug() <<__FUNCTION__ <<"Checking ACTION: " << QString::fromStdString(action);

	static std::string previousAction = "";
	bool newAction = (previousAction != action);

	if (newAction)
	{
		printf("prev:%s  new:%s\n", previousAction.c_str(), action.c_str());
		rDebug2(("action %s") % action.c_str() );
	}

	if (action == "changeroom")
	{
		action_ChangeRoom(newAction);
	}
	else if (action == "findobjectvisuallyintable")
	{
		action_FindObjectVisuallyInTable(newAction);
	}
	else if (action == "setobjectreach" or action == "graspobject")
	{
		action_SetObjectReach(newAction);
	}
	else if (action == "detectperson")
	{
		action_DetectPerson(newAction);
	}
	else if (action == "handobject")
	{
//		action_HandObject(newAction);
	}
	else if (action == "setstop")
	{
		action_Stop();
	}
	else if (action == "reachpose")
	{
		action_ReachPose();
	}
	else if (action == "noAction")
	{

	}
	else if (action == "handobject_offer")
	{
		action_HandObject_Offer();
	}
	else if (action == "handobject_leave")
	{
		action_HandObject_leave();
	}
	if (newAction)
	{
		previousAction = action;
		printf("New action: %s\n", action.c_str());
	}
	// manageReachedPose();
// 	printf("actionExecution>>\n");
}

void ActionExecution::action_HandObject_leave(bool newAction)
{
	try
	{
		trajectoryrobot2d_proxy->stop();
		omnirobot_proxy->setSpeedBase(0.,0.,0.0);
	}
	catch(...)
	{
		printf("Can't stop the robot!!\n");
	}
}
void ActionExecution::action_DetectPerson(bool newAction)
{

	static bool b=false;
	if (b==false)
        {
		trajectoryrobot2d_proxy->stop();
		b=true;
	}
	try
	{
		omnirobot_proxy->setSpeedBase(0.,0.,0.1);
	}
	catch(...)
	{
		printf("Can't connect to the robot!!\n");
	}

}

//////////
// Slot
//////////
void ActionExecution::readTrajState()
{
	try
	{
		planningState = trajectoryrobot2d_proxy->getState();
		std::cout << "State:" + planningState.state + ". Description: " + planningState.description << std::endl;
	}
	catch(const Ice::Exception &ex)
	{
		std::cout << ex << "Error talking to TrajectoryRobot2D" <<  std::endl;
	}
	catch(...)
	{
		printf("something else %d\n", __LINE__);
	}
}

void ActionExecution::action_HandObject(bool newAction)
{
	// Get symbols' map
	std::map<std::string, AGMModelSymbol::SPtr> symbols;
	try
	{
		symbols = worldModel->getSymbolsMap(params/*,  "robot", "room", "object", "status"*/); //ALL THE SYMBOLS GIVEN IN THE RULE
	}
	catch(...)
	{
		printf("navigationAgent, action_HandObject: Couldn't retrieve action's parameters\n");
		printf("<<WORLD\n");
		AGMModelPrinter::printWorld(worldModel);
		printf("WORLD>>\n");
		if (worldModel->size() > 0) { exit(-1); }
	}

	// Get target
	int roomID, personID, robotID;
	try
	{
		if (symbols["room"].get() and symbols["person"].get() and symbols["robot"].get())
		{
			roomID = symbols["room"]->identifier;   //7 ROOM
			personID =symbols["person"]->identifier;//  PERSON
			robotID = symbols["robot"]->identifier; //1 ROBOT
		}
		else
		{
			printf("navigationAgent, action_HandObject: parameters not in the model yet\n");
			return;
		}
	}
	catch(...)
	{
		printf("navigationAgent, action_HandObject ERROR: SYMBOL DOESN'T EXIST \n");
		exit(2);
	}

	// GET THE INNERMODEL NAMES OF TH SYMBOLS
	QString robotIMID;
	QString roomIMID;
	QString personIMID;
	try
	{
		robotIMID = QString::fromStdString(worldModel->getSymbol(robotID)->getAttribute("imName"));
		roomIMID = QString::fromStdString(worldModel->getSymbol(roomID)->getAttribute("imName"));
		//we need to obtain the imName of the torso node. TrackingId+"XN_SKEL_TORSO"
		QString trackingId= QString::fromStdString(worldModel->getSymbol(personID)->getAttribute("TrackingId"));
		personIMID = trackingId +"XN_SKEL_TORSO";
	}
	catch(...)
	{
		printf("navigationAgent, action_HandObject: ERROR IN GET THE INNERMODEL NAMES\n");
		qDebug()<<"[robotIMID"<<robotIMID<<"roomIMID"<<roomIMID<<"personIMID"<<personIMID<<"]";
		exit(2);
	}

	// GET THE TARGET POSE:
	RoboCompTrajectoryRobot2D::TargetPose tgt;
	try
	{
		if (not (innerModel->getNode(roomIMID) and innerModel->getNode(personIMID)))    return;

		QVec poseInRoom = innerModel->transform6D(roomIMID, personIMID); // FROM OBJECT TO ROOM
		qDebug()<<"[robotIMID"<<robotIMID<<"roomIMID"<<roomIMID<<"personIMID"<<personIMID<<"]";
		qDebug()<<" TARGET POSE: "<< poseInRoom;

		tgt.x = poseInRoom.x();
		tgt.y = 0;
		tgt.z = poseInRoom.z();
		tgt.rx = 0;
		tgt.ry = 0;
		tgt.rz = 0;
		tgt.doRotation = false;

	}
	catch (...)
	{
		qDebug()<<"navigationAgent, action_HandObject: innerModel exception";
	}

	try
	{
// 		if (!haveTarget)
		{
			try
			{
				QVec graspRef = innerModel->transform("robot", "shellyArm_grasp_pose");
				float th=20;
				go(tgt.x, tgt.z, tgt.ry, tgt.doRotation, graspRef.x(), graspRef.z(), th);
				std::cout << "trajectoryrobot2d->go(" << currentTarget.x << ", " << currentTarget.z << ", " << currentTarget.ry << ", " << currentTarget.doRotation << ", " << graspRef.x() << ", " << graspRef.z() << " )\n";
				haveTarget = true;
			}
			catch(const std::exception &ex)
			{
				std::cout <<"navigationAgent, action_HandObject: ERROR trajectoryrobot2d->go "<< ex.what() << std::endl;
				throw ex;
			}
		}
		string state;
		try
		{
			state = trajectoryrobot2d_proxy->getState().state;
		}
		catch(const std::exception &ex)
		{
			std::cout <<"navigationAgent, action_HandObject: trajectoryrobot2d->getState().state "<< ex.what() << std::endl;
			throw ex;
		}

		//state="IDLE";
		std::cout<<state<<" haveTarget "<<haveTarget;
		if (state=="IDLE" && haveTarget)
		{
			//std::cout<<"\ttrajectoryrobot2d_proxy->getState() "<<trajectoryrobot2d_proxy->getState().state<<"\n";
			try
			{
// 				AGMModel::SPtr newModel(new AGMModel(worldModel));
// 				int statusID =symbols["status"]->identifier;
// 				newModel->getEdgeByIdentifiers(objectID, statusID, "noReach").setLabel("reach");
// 				sendModificationProposal(worldModel, newModel);
				haveTarget=false;
			}
			catch (...)
			{
				std::cout<<"\neeeee"<< "\n";
			}
		}
	}
	catch(const std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
	}


}

/**
*  \brief Called when the robot is sent close to a person to offer the object
*/
void ActionExecution::action_HandObject_Offer(bool newAction)
{
	// Get symbols' map
	std::map<std::string, AGMModelSymbol::SPtr> symbols;
	try
	{
		symbols = worldModel->getSymbolsMap(params/*,  "robot", "room", "object", "status"*/); //ALL THE SYMBOLS GIVEN IN THE RULE

	}
	catch(...)
	{
		printf("navigationAgent: Couldn't retrieve action's parameters\n");
		printf("<<WORLD\n");
		AGMModelPrinter::printWorld(worldModel);
		printf("WORLD>>\n");
		if (worldModel->size() > 0) { exit(-1); }
	}

	// Get target
	int roomID, personID, robotID;
	try
	{
		if (symbols["room"].get() and symbols["person"].get() and symbols["robot"].get() and symbols["status"].get())
		{
			roomID = symbols["room"]->identifier;
			personID =symbols["person"]->identifier;
			robotID = symbols["robot"]->identifier;

			try // If we can access the 'reach' edge for the object status the action
			{   // is not really necessary. The planner is probably replanning.
				worldModel->getEdgeByIdentifiers(personID, symbols["status"]->identifier, "reach");
				{
					static QTime lastMsg = QTime::currentTime().addSecs(-1000);
					if (lastMsg.elapsed() > 1000)
					{
						rDebug2(("navigationAgent ignoring action setHandObject_Offer (person currently reached)"));
						lastMsg = QTime::currentTime();
						return;
					}
					printf("ask the platform to stop\n");
					stop();
				}
			}
			catch(...)
			{
			}

		}
		else
		{
			printf("parameters not in the model yet\n");
			return;
		}
	}
	catch(...)
	{
		printf("ERROR: SYMBOL DOESN'T EXIST \n");
		exit(2);
	}


	// GET THE INNERMODEL NAMES OF TH SYMBOLS
	QString robotIMID;
	QString roomIMID;
	QString personIMID;
	try
	{
		robotIMID = QString::fromStdString(worldModel->getSymbol(robotID)->getAttribute("imName"));
		roomIMID = QString::fromStdString(worldModel->getSymbol(roomID)->getAttribute("imName"));
		personIMID = QString::fromStdString(worldModel->getSymbol(personID)->getAttribute("imName"));
	}
	catch(...)
	{
		printf("ERROR IN GET THE INNERMODEL NAMES\n");
		exit(2);
	}

	// GET THE TARGET POSE:
	RoboCompTrajectoryRobot2D::TargetPose tgt;
	try
	{
		if (not (innerModel->getNode(roomIMID) and innerModel->getNode(personIMID)))    return;
		QVec poseInRoom = innerModel->transform6D(roomIMID, personIMID); // FROM PERSON TO ROOM
		qDebug() << __FUNCTION__ <<" Target pose: "<< poseInRoom;

		tgt.x = poseInRoom.x();
		tgt.y = 0;
		tgt.z = poseInRoom.z();
		tgt.rx = 0;
		tgt.ry = poseInRoom.ry();
		tgt.rz = 0;
		tgt.doRotation = true;
	}
	catch (...)
	{
		qDebug()<< __FUNCTION__ << "InnerModel Exception. Element not found in tree";
	}

	// Execute target
	try
	{
		try
		{
			QVec O = innerModel->transform("shellyArm_grasp_pose", personIMID);
			QVec graspRef = innerModel->transform("robot", "shellyArm_grasp_pose");
			go(tgt.x, tgt.z, -3.141592, tgt.doRotation, graspRef.x(), graspRef.z(), 20);
			qDebug() << __FUNCTION__ << "trajectoryrobot2d->go(" << tgt.x << ", " << tgt.z << ", " << tgt.ry << ", " << graspRef.x() << ", " << graspRef.z() << " )\n";
			haveTarget = true;
		}
		catch(const RoboCompTrajectoryRobot2D::RoboCompException &ex)
		{
			std::cout << ex << " " << ex.text << std::endl;
			throw;
		}
		catch(const std::exception &ex)
		{
			std::cout << ex.what() << std::endl;
		}
	}
	catch(const std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
	}


}


/**
*  \brief Called when the robot is sent close to an object's location
*/
void ActionExecution::action_SetObjectReach(bool newAction)
{	// Get symbols' map
	std::map<std::string, AGMModelSymbol::SPtr> symbols;
	try
	{
		symbols = worldModel->getSymbolsMap(params/*,  "robot", "room", "object", "status"*/); //ALL THE SYMBOLS GIVEN IN THE RULE
	}
	catch(...)
	{
		printf("navigationAgent: Couldn't retrieve action's parameters\n");
		printf("<<WORLD\n");
		AGMModelPrinter::printWorld(worldModel);
		printf("WORLD>>\n");
		if (worldModel->size() > 0) { exit(-1); }
	}

	// Get target
	int roomID, objectID, robotID;
	try
	{
		if (symbols["room"].get() and symbols["object"].get() and symbols["robot"].get() and symbols["status"].get())
		{
			roomID = symbols["room"]->identifier;
			objectID =symbols["object"]->identifier;
			robotID = symbols["robot"]->identifier;

			try // If we can access the 'reach' edge for the object status the action
			{   // is not really necessary. The planner is probably replanning.
				worldModel->getEdgeByIdentifiers(objectID, symbols["status"]->identifier, "reach");
				{
					static QTime lastMsg = QTime::currentTime().addSecs(-1000);
					if (lastMsg.elapsed() > 1000)
					{
						rDebug2(("navigationAgent ignoring action setObjectReach (object currently reached)"));
						lastMsg = QTime::currentTime();
						return;
					}
					printf("ask the platform to stop\n");
					stop();
				}
			}
			catch(...)
			{
			}

		}
		else
		{
			printf("parameters not in the model yet\n");
			return;
		}
	}
	catch(...)
	{
		printf("ERROR: SYMBOL DOESN'T EXIST \n");
		exit(2);
	}

	// GET THE INNERMODEL NAMES OF TH SYMBOLS
	QString robotIMID;
	QString roomIMID;
	QString objectIMID;
	try
	{
		robotIMID = QString::fromStdString(worldModel->getSymbol(robotID)->getAttribute("imName"));
		roomIMID = QString::fromStdString(worldModel->getSymbol(roomID)->getAttribute("imName"));
		objectIMID = QString::fromStdString(worldModel->getSymbol(objectID)->getAttribute("imName"));

		// check if object has reachPosition
		AGMModelSymbol::SPtr object = worldModel->getSymbol(objectID);
		for (auto edge = object->edgesBegin(worldModel); edge != object->edgesEnd(worldModel); edge++)
		{
			if (edge->getLabel() == "reachPosition")
			{
				const std::pair<int32_t, int32_t> symbolPair = edge->getSymbolPair();
				objectID = symbolPair.second;
				objectIMID = QString::fromStdString(worldModel->getSymbol(objectID)->getAttribute("imName"));
				qDebug() << __FUNCTION__ << "Target object " << symbolPair.first<<"->"<<symbolPair.second<<" object "<<objectIMID;
			}
		}
	}
	catch(...)
	{
		printf("ERROR IN GET THE INNERMODEL NAMES\n");
		exit(2);
	}

	// GET THE TARGET POSE:
	RoboCompTrajectoryRobot2D::TargetPose tgt;
	try
	{
		if (not (innerModel->getNode(roomIMID) and innerModel->getNode(objectIMID)))    return;
		QVec poseInRoom = innerModel->transform6D(roomIMID, objectIMID); // FROM OBJECT TO ROOM
		qDebug() << __FUNCTION__ <<" Target pose: "<< poseInRoom;

		tgt.x = poseInRoom.x();
		tgt.y = 0;
		tgt.z = poseInRoom.z();
		tgt.rx = 0;
		//tgt.ry = 0;
		tgt.ry = poseInRoom.ry(); //needed to reach tables
		tgt.rz = 0;
		tgt.doRotation = true;
	}
	catch (...)
	{
		qDebug()<< __FUNCTION__ << "InnerModel Exception. Element not found in tree";
	}

	// Execute target
	try
	{
// 		if (!haveTarget)
		{
			try
			{
				QVec O = innerModel->transform("shellyArm_grasp_pose", objectIMID);
				//O.print("	O pose relativa");
				//qDebug() << __FUNCTION__ << "O norm:" << O.norm2();
				QVec graspRef = innerModel->transform("robot", "shellyArm_grasp_pose");
        graspRef.print("graspRef");
				float th=20;
        tgt.ry = -3.1415926535/2.;
				go(tgt.x, tgt.z, tgt.ry, tgt.doRotation, graspRef.x(), graspRef.z(), th);
				qDebug() << __FUNCTION__ << "trajectoryrobot2d->go(" << tgt.x << ", " << tgt.z << ", " << tgt.ry << ", " << graspRef.x() << ", " << graspRef.z() << " )\n";
				haveTarget = true;
			}
			catch(const RoboCompTrajectoryRobot2D::RoboCompException &ex)
			{
				std::cout << ex << " " << ex.text << std::endl;
				throw;
			}
			catch(const std::exception &ex)
			{
				std::cout << ex.what() << std::endl;
			}
		}
		string state;
		try
		{
			state = trajectoryrobot2d_proxy->getState().state;
		}
		catch(const std::exception &ex)
		{
			std::cout <<"trajectoryrobot2d->getState().state "<< ex.what() << std::endl;
			throw ex;
		}

		//state="IDLE";
		std::cout<<state<<" haveTarget "<<haveTarget;
		if (state=="IDLE" && haveTarget)
		{
			//std::cout<<"\ttrajectoryrobot2d_proxy->getState() "<<trajectoryrobot2d_proxy->getState().state<<"\n";
			try
			{
// 				AGMModel::SPtr newModel(new AGMModel(worldModel));
// 				int statusID =symbols["status"]->identifier;
// 				newModel->getEdgeByIdentifiers(objectID, statusID, "noReach").setLabel("reach");
// 				sendModificationProposal(worldModel, newModel);
				haveTarget=false;
			}
			catch (...)
			{
				std::cout<<"\neeeee"<< "\n";
			}
		}
	}
	catch(const std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
	}
}

void ActionExecution::manageReachedPose()
{
	float schmittTriggerThreshold = 20;
	float THRESHOLD_POSE = 100;
	std::string m ="  ";

	bool changed = false;

	QMutexLocker locker(mx);

	AGMModel::SPtr newModel(new AGMModel(worldModel));

	for (AGMModel::iterator symbol_itr=newModel->begin(); symbol_itr!=newModel->end(); symbol_itr++)
	{
		AGMModelSymbol::SPtr node = *symbol_itr;
		if (node->symboltype() == "pose")
		{
			/// Compute distance and new state
			float d2n;
			try
			{
				QVec arm = innerModel->transformS("world", "robot");
				QVec obj = innerModel->transformS("world", node->getAttribute("imName"));
				(arm-obj);//.print("error");

				d2n = distanceToNode("robot", newModel, node);
//qDebug()<<"distance "<<d2n;
			}
			catch(...)
			{
				printf("Ref: robot: %p\n", (void *)innerModel->getNode("robot"));
				printf("Pose: %s: %p\n", node->getAttribute("imName").c_str(), (void *)innerModel->getNode(node->getAttribute("imName").c_str()));
				exit(1);
			}


			for (AGMModelSymbol::iterator edge_itr=node->edgesBegin(newModel); edge_itr!=node->edgesEnd(newModel); edge_itr++)
			{
				AGMModelEdge &edge = *edge_itr;


//*********** FIX
				if (edge->getLabel() == "reach")
				{
					//TODO: FIX ==> Update wasIn mug status
						AGMModelSymbol::SPtr mug = newModel->getSymbolByIdentifier(50); //50 ==> bluemug
						int32_t inTableID = -1;
						int32_t wasInTableID = -1;
						for (auto edgeMug = mug->edgesBegin(newModel); edgeMug != mug->edgesEnd(newModel); edgeMug++)
						{
							if(newModel->getSymbolByIdentifier(edgeMug->getSymbolPair().second)->symboltype() == "object")
							{
								printf("symbol: %d\n", edgeMug->getSymbolPair().second);
								bool doChange = false;
								AGMModelSymbol::SPtr container = newModel->getSymbolByIdentifier(edgeMug->getSymbolPair().second);
								for (auto edgeMug2 = container->edgesBegin(newModel); edgeMug2 != container->edgesEnd(newModel); edgeMug2++)
								{
									if (edgeMug2->getLabel() == "table")
										doChange = true;
								}
								if (doChange)
								{
									printf("doChange\n");
									if (edgeMug->getLabel() == "wasIn")
									{
										wasInTableID = edgeMug->getSymbolPair().second;
									}
									else if (edgeMug->getLabel() == "in")
									{
										inTableID = edgeMug->getSymbolPair().second;
									}
								}
							}
						}
qDebug()<<"inTable"<<inTableID<<"was"<<wasInTableID;
						if(inTableID != -1 and wasInTableID != -1 and inTableID != wasInTableID)
						{
							changed = true;
							newModel->removeEdgeByIdentifiers(mug->identifier, wasInTableID, "wasIn");
							newModel->addEdgeByIdentifiers( mug->identifier, inTableID, "wasIn");
						}
				}
//***************

				if (edge->getLabel() == "reach" and d2n > THRESHOLD_POSE+schmittTriggerThreshold )
				{
					edge->setLabel("noReach");
					printf("pose %d STOPS REACH\n", node->identifier);
					m += " action " + action + " edge->toString() "+ edge->toString(newModel);
					changed = true;
					rDebug2(("pose %d no-reach") % node->identifier);
				}
				else if (edge->getLabel() == "noReach" and d2n < THRESHOLD_POSE/*-schmittTriggerThreshold*/)
				{
					edge->setLabel("reach");
					printf("___ %s ___\n", edge->getLabel().c_str());
					printf("pose %d STARTS REACH\n", node->identifier);
					m += " action " + action + " edge->toString() "+ edge->toString(newModel);
					changed = true;
					rDebug2(("pose %d reach") % node->identifier);
				}
			}
		}
	}

	/// Publish new model if changed
	if (changed)
	{
		printf("PUBLISH!!!! version%d\n", newModel->version);
		sendModificationProposal(newModel, worldModel, m);
	}
}


float ActionExecution::distanceToNode(std::string reference_name, AGMModel::SPtr model, AGMModelSymbol::SPtr object)
{
	QMutexLocker locker(mx);

	QVec arm = innerModel->transformS("world", reference_name);
	arm(1) = 0;
	QVec obj = innerModel->transformS("world", object->getAttribute("imName"));
	obj(1) = 0;
	return (arm-obj).norm2();

}

int32_t ActionExecution::getIdentifierOfRobotsLocation(AGMModel::SPtr &worldModel)
{
	for (AGMModel::iterator symbol_it=worldModel->begin(); symbol_it!=worldModel->end(); symbol_it++)
	{
		const AGMModelSymbol::SPtr &symbol = *symbol_it;
		if (symbol->symbolType == "robot")
		{
			for (AGMModelSymbol::iterator edge_it=symbol->edgesBegin(worldModel); edge_it!=symbol->edgesEnd(worldModel); edge_it++)
			{
				AGMModelEdge edge = *edge_it;
				if (edge.linking == "in")
				{
					return edge.symbolPair.second;
				}
			}
		}
	}

	printf("wheres's the robot?\n");
	return -1;
}

void ActionExecution::setIdentifierOfRobotsLocation(AGMModel::SPtr &model, int32_t identifier)
{
	bool didSomethin = false;
	for (AGMModel::iterator symbol_it=worldModel->begin(); symbol_it!=worldModel->end(); symbol_it++)
	{
		const AGMModelSymbol::SPtr &symbol = *symbol_it;
		if (symbol->symbolType == "robot")
		{

			for (AGMModelSymbol::iterator edge_it=symbol->edgesBegin(worldModel); edge_it!=symbol->edgesEnd(worldModel); edge_it++)
			{
				if (edge_it->linking == "in")
				{
					printf("it was %d\n", edge_it->symbolPair.second);
				}
			}
			for (int32_t edgeIndex=0; edgeIndex<model->numberOfEdges(); edgeIndex++)
			{
				if (model->edges[edgeIndex].linking == "in")
				{
					if (model->edges[edgeIndex].symbolPair.first == symbol->identifier)
					{
						model->edges[edgeIndex].symbolPair.second = identifier;
						didSomethin = true;
					}
				}
			}
			for (AGMModelSymbol::iterator edge_it=symbol->edgesBegin(worldModel); edge_it!=symbol->edgesEnd(worldModel); edge_it++)
			{
				if (edge_it->linking == "in")
				{
					printf("now is %d\n", edge_it->symbolPair.second);
				}
			}
		}
	}
	if (not didSomethin)
		qFatal("couldn't update robot's room in the cog graph");
}

void ActionExecution::action_ChangeRoom(bool newAction)
{
	static float lastX = std::numeric_limits<float>::quiet_NaN();
	static float lastZ = std::numeric_limits<float>::quiet_NaN();

	auto symbols = worldModel->getSymbolsMap(params, "r2", "robot");


	try
	{
		printf("trying to get _in_ edge from %d to %d\n", symbols["robot"]->identifier, symbols["r2"]->identifier);
		AGMModelEdge e = worldModel->getEdge(symbols["robot"], symbols["r2"], "in");
		printf("STOP! WE ALREADY GOT THERE!\n");
		stop();
		return;
	}
	catch(...)
	{
	}

	int32_t roomId = symbols["r2"]->identifier;
	printf("room symbol: %d\n",  roomId);
	std::string imName = symbols["r2"]->getAttribute("imName");
	printf("imName: <%s>\n", imName.c_str());

	const float refX = str2float(symbols["r2"]->getAttribute("x"));
	const float refZ = str2float(symbols["r2"]->getAttribute("z"));

	QVec roomPose = innerModel->transformS("world", QVec::vec3(refX, 0, refZ), imName);
	roomPose.print("goal pose");
	// 	AGMModelSymbol::SPtr goalRoom = worldModel->getSymbol(str2int(params["r2"].value));
	// 	const float x = str2float(goalRoom->getAttribute("tx"));
	// 	const float z = str2float(goalRoom->getAttribute("tz"));
	const float x = roomPose(0);
	const float z = roomPose(2);

	bool proceed = true;
	if ( (planningState.state=="PLANNING" or planningState.state=="EXECUTING") )
	{
		if (abs(lastX-x)<10 and abs(lastZ-z)<10)
			proceed = false;
		else
			printf("proceed because the coordinates differ (%f, %f), (%f, %f)\n", x, z, lastX, lastZ);
	}
	else
	{
		printf("proceed because it's stoped\n");
	}

	if (proceed)
	{
		lastX = x;
		lastZ = z;
		printf("changeroom to %d\n", symbols["r2"]->identifier);
		go(x, z, 0, false, 0, 0, 1200);
	}
	else
	{
	}
}

void ActionExecution::action_Stop(bool newAction)
{
	stop();
}

void ActionExecution::action_ReachPose(bool newAction)
{
	printf("action_ReachPose,%d: %d\n", __LINE__, newAction);
	static float lastX = std::numeric_limits<float>::quiet_NaN();
	printf("action_ReachPose,%d: %d\n", __LINE__, newAction);
	static float lastZ = std::numeric_limits<float>::quiet_NaN();
	printf("action_ReachPose,%d: %d\n", __LINE__, newAction);

	auto symbols = worldModel->getSymbolsMap(params, "room", "pose");
	printf("action_ReachPose,%d: %d\n", __LINE__, newAction);

	int32_t poseId = symbols["pose"]->identifier;
	printf("pose symbol: %d\n",  poseId);
	std::string imName = symbols["pose"]->getAttribute("imName");
	printf("imName: <%s>\n", imName.c_str());

	QVec pose = innerModel->transform6D("world", QString::fromStdString(imName));
	pose.print("goal pose");
	const float x = pose(0);
	const float z = pose(2);
	const float ry = pose(4);

	bool proceed = true;
	if ( (planningState.state=="PLANNING" or planningState.state=="EXECUTING") )
	{
		if (abs(lastX-x)<10 and abs(lastZ-z)<10)
			proceed = false;
		else
			printf("proceed because the coordinates differ (%f, %f), (%f, %f)\n", x, z, lastX, lastZ);
	}
	else
	{
		printf("proceed because it's stoped\n");
	}

	if (proceed)
	{
		lastX = x;
		lastZ = z;
		printf("setpose %d\n", symbols["room"]->identifier);
		go(x, z, ry, true);
	}
	else
	{
	}
}

void ActionExecution::action_FindObjectVisuallyInTable(bool newAction)
{
// 	stop();


	static float lastX = std::numeric_limits<float>::quiet_NaN();
	static float lastZ = std::numeric_limits<float>::quiet_NaN();

	AGMModelSymbol::SPtr goalTable;
	AGMModelSymbol::SPtr robot;
	int32_t tableId = -1;
	try
	{
		tableId = str2int(params["container"].value);
		goalTable = worldModel->getSymbol(tableId);
		robot = worldModel->getSymbol(worldModel->getIdentifierByType("robot"));
	}
	catch(...)
	{
		printf("can't access robot or table\n");
		return;
	}

	const float x = str2float(goalTable->getAttribute("tx"));
	const float z = str2float(goalTable->getAttribute("tz"));
	float alpha = tableId==7?-3.141592:0;
// printf("%s: %d\n", __FILE__, __LINE__);

	const float rx = str2float(robot->getAttribute("tx"));
	const float rz = str2float(robot->getAttribute("tz"));
	const float ralpha = str2float(robot->getAttribute("alpha"));
// printf("%s: %d\n", __FILE__, __LINE__);

	// Avoid repeating the same goal and confuse the navigator
	const float errX = abs(rx-x);
	const float errZ = abs(rz-z);
	float errAlpha = abs(ralpha-alpha);
	while (errAlpha > +M_PIl) errAlpha -= 2.*M_PIl;
	while (errAlpha < -M_PIl) errAlpha += 2.*M_PIl;
	errAlpha = abs(errAlpha);
	if (errX<20 and errZ<20 and errAlpha<0.02)
		return;
// printf("%s: %d\n", __FILE__, __LINE__);

	bool proceed = true;
	if ( (planningState.state=="IDLE")) //PLANNING" or planningState.state=="EXECUTING") )
	{
		if (abs(lastX-x)<10 and abs(lastZ-z)<10)
			proceed = false;
		else
			printf("proceed because the coordinates differ (%f, %f), (%f, %f)\n", x, z, lastX, lastZ);
	}
	else
	{
		printf("proceed because it's stoped\n");
	}

	if (proceed)
	{
		lastX = x;
		lastZ = z;
		printf("find objects in table %d\n", tableId);
		go(x, tableId==7?z+550:z-550, tableId==7?-3.141592:0, true, 0, 500, 500);
	}
	else
	{
	}
}

void ActionExecution::action_NoAction(bool newAction)
{
	try{
		std::string state = trajectoryrobot2d_proxy->getState().state;
		if (state != "IDLE")
		{
			printf("trajectoryrobot2d state : %s\n", state.c_str());
			stop();
		}
	}catch(...)
	{
		std::cout<< "Error retrieving trajectory state"<<std::endl;
	}
}


// Send target to trajectory
void ActionExecution::go(float x, float z, float alpha, bool rot, float xRef, float zRef, float threshold)
{
	static bool first = true;

	RoboCompTrajectoryRobot2D::TargetPose lastTarget = currentTarget;

	currentTarget.x = x;
	currentTarget.z = z;
	currentTarget.y = 0;
	currentTarget.rx = 0;
	currentTarget.rz = 0;

	if (rot)
	{
		currentTarget.ry = alpha;
		currentTarget.doRotation = true;
	}
	else
	{
		currentTarget.ry = 0;
		currentTarget.doRotation = false;
	}


	if ((not first) and (lastTarget == currentTarget))
		return;

	try
	{
		std::cout<< "ENVIANDO A trajectoryrobot2d->go(" << currentTarget.x << ", " << currentTarget.z << ", " << currentTarget.ry << ", " << xRef << ", " << zRef << ", " << threshold << " )" <<std::endl;
		trajectoryrobot2d_proxy->goReferenced(currentTarget, xRef, zRef, threshold);
	}
	catch(const RoboCompTrajectoryRobot2D::RoboCompException &ex)
	{
		std::cout << ex.text << std::endl;
	}
	catch(const std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
	}
	catch(...)
	{
		printf("Exception: something else %d\n", __LINE__);
	}
}


void ActionExecution::stop()
{
	try
	{
		trajectoryrobot2d_proxy->stop();
	}
	catch(const std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
	}
	catch(...)
	{
		printf("Exception: something else %d\n", __LINE__);
	}
}



// ////////////////////////////////////////////////////////////////////////////////////////////////////////////4

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ActionExecution::structuralChange(const RoboCompAGMWorldModel::World& modification)
{
	printf("pre <<structuralChange\n");
	//QMutexLocker l(mx);
	printf("<<structuralChange\n");

	AGMModelConverter::fromIceToInternal(modification, worldModel);
	//if (roomsPolygons.size()==0 and worldModel->numberOfSymbols()>0)
		//roomsPolygons = extractPolygonsFromModel(worldModel);

	//if (innerModel) delete innerModel;
	//innerModel = AGMInner::extractInnerModel(worldModel, "world", true);
	InnerModel *inner = AGMInner::extractInnerModel(worldModel, "world", true);
	innerModel.reset(inner);
	printf("structuralChange>>\n");
}

void ActionExecution::sendModificationProposal(AGMModel::SPtr &newModel, AGMModel::SPtr &worldModel, string m)
{
	QMutexLocker locker(mx);

	try
	{
		AGMMisc::publishModification(newModel, agmexecutive_proxy, std::string( "graspingAgent")+m);
	}
	catch(const RoboCompAGMExecutive::Locked &e)
	{
	}
	catch(const RoboCompAGMExecutive::OldModel &e)
	{
	}
	catch(const RoboCompAGMExecutive::InvalidChange &e)
	{
	}
	catch(const std::exception& e)
	{
		exit(1);
	}
}

