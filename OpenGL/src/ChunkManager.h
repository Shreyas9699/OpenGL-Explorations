#include <queue>
#include <mutex>
#include <condition_variable>
#include "TerrainChunk.h"

class ChunkManager
{
public:
	void AddLoadTask(TerrainChunk* chunk)
	{
		std::lock_guard<std::mutex> lock(m_QueueMutex);
		m_LoadQueue.push(chunk);
		m_Condition.notify_one();
	}

	void ProcessTasks()
	{
		while (true)
		{
			std::unique_lock<std::mutex> lock(m_QueueMutex);
			m_Condition.wait(lock, [&] { return !m_LoadQueue.empty(); });

			auto chunk = m_LoadQueue.front();
			m_LoadQueue.pop();
			lock.unlock();

			if (chunk->m_State == TerrainChunk::UNLOADED)
			{
				chunk->m_State = TerrainChunk::LOADING;
				chunk->GenerateMesh();
				chunk->m_State = TerrainChunk::LOADED;
			}
		}
	}

private:
	std::queue<TerrainChunk*> m_LoadQueue;
	std::mutex m_QueueMutex;
	std::condition_variable  m_Condition;
};