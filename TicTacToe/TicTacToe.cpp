#include "OGRE\Ogre.h"
#include "OIS\OIS.h"

using namespace Ogre;

class TTTFrameListener : public FrameListener
{

	private:

		OIS::InputManager* mInputMgr;
		OIS::Keyboard* mKeyboard;

	public:

		TTTFrameListener(RenderWindow* win)
		{
			OIS::ParamList params;
			unsigned int windowHandle = 0;
			std::ostringstream windowHandleString;
			win->getCustomAttribute("WINDOW", &windowHandle);
			windowHandleString << windowHandle;
			params.insert(std::make_pair("WINDOW", windowHandleString.str()));

			mInputMgr = OIS::InputManager::createInputSystem(params);
			mKeyboard = static_cast<OIS::Keyboard*>(mInputMgr->createInputObject(OIS::OISKeyboard, false));

		}

		~TTTFrameListener()
		{
			mInputMgr->destroyInputObject(mKeyboard);
			OIS::InputManager::destroyInputSystem(mInputMgr);
		}
		
		bool frameStarted(const Ogre::FrameEvent& evt)
		{
			mKeyboard->capture();
			if(mKeyboard->isKeyDown(OIS::KC_ESCAPE))
				return false;
			return true;
		}

};

class TicTacToe
{

	private:
		SceneManager* mSceneMgr;
		Root* mRoot;
		TTTFrameListener* mListener;
		bool mKeepRunning;

	public:

		TicTacToe()
		{

			mSceneMgr = NULL;
			mListener = NULL;

		}

		~TicTacToe()
		{

			delete mRoot;
			delete mListener;

		}

		void loadResources()
		{

			ConfigFile cf;
#if OGRE_DEBUG_MODE
			cf.load("resources_d.cfg");
#else
			cf.load("resources.cfg");
#endif
			ConfigFile::SectionIterator sectIter = cf.getSectionIterator();
			String sectName, typeName, dataName;
			while (sectIter.hasMoreElements())
			{

				sectName = sectIter.peekNextKey();
				ConfigFile::SettingsMultiMap *settings = sectIter.getNext();
				Ogre::ConfigFile::SettingsMultiMap::iterator i;

				for(i=settings->begin(); i != settings->end(); i++)
				{
					typeName = i->first;
					dataName = i->second;
					ResourceGroupManager::getSingleton().addResourceLocation(dataName,typeName,sectName);
				}
			}

			ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

		}

		int startup ()
		{
#if OGRE_DEBUG_MODE
			mRoot = new Root("plugins_d.cfg");
#else
			mRoot = new Root("plugins.cfg");
#endif
			if(!mRoot->showConfigDialog())
			{
				return -1;
			}

			RenderWindow* window = mRoot->initialise(true,"TicTacToe");

			mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

			Camera* camera = mSceneMgr->createCamera("Camera");

			camera->setPosition(Vector3(0,0,50));
			camera->lookAt(Vector3(0,0,0));
			camera->setNearClipDistance(5);

			Viewport* viewport = window->addViewport(camera);
			viewport->setBackgroundColour(ColourValue(57.0/255.0, 251.0/255.0, 232.0/255.0));

			camera->setAspectRatio(Real(viewport->getActualWidth())/Real(viewport->getActualHeight()));

			loadResources();
			createScene();

			mListener = new TTTFrameListener(window);
			mRoot->addFrameListener(mListener);

			return 0;

		}

		void renderOneFrame()
		{
			WindowEventUtilities::messagePump();
			mKeepRunning = mRoot->renderOneFrame();
		}

		bool keepRunning()
		{
			return mKeepRunning;
		}

		void createScene ()
		{

			Entity* ent = mSceneMgr->createEntity("Sinbad.mesh");
			mSceneMgr->getRootSceneNode()->attachObject(ent);

		}

};

int main()
{

	TicTacToe app;
	app.startup();

	while(app.keepRunning())
	{
		app.renderOneFrame();
	}

	return 0;

}