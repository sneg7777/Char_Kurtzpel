#pragma once
class CRandom_Manager
{
public:
	static CRandom_Manager* Get_Instance() {
		if (nullptr == m_pInstance) {
			m_pInstance = new CRandom_Manager;
		}
		return m_pInstance;
	}
	static void Destroy_Instance() {
		if (m_pInstance) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

private:
	CRandom_Manager();
	~CRandom_Manager();

public:
	void Ready_Random() { srand(unsigned(time(nullptr))); }
	static int Random() {
		int iTemp;
		iTemp = rand();
		return iTemp;
	}

private:
	static CRandom_Manager* m_pInstance;
};

