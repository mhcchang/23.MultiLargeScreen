#include "WidgetVideoSegment.h"

WidgetVideoSegment::WidgetVideoSegment(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	memset(m_widgetCams, 0, sizeof(WidgetCam*) * M_MAX_SEG);
	m_nSegment = 4;
	m_nCurrentPos = 0;
	SetSegmentCount(m_nSegment);
}

WidgetVideoSegment::~WidgetVideoSegment()
{
}

bool WidgetVideoSegment::SetSegmentCount(int ns)
{
	double dqs = sqrt(ns);
	if (dqs * dqs - ns > EPSILON_FLOAT)
		return false;
	if (m_nSegment == ns)
		return true;
	m_nSegment = ns;
	int nqs = dqs;

	QRect rt = this->geometry();
	
	switch (nqs)
	{
	case 1:
		if (!m_widgetCams[0])
		{
			m_widgetCams[0] = new WidgetCam(this);
			m_widgetCams[0]->SetIndex(0);
			connect(m_widgetCams[0], &WidgetCam::IndexChangeSignal, this, &WidgetVideoSegment::ChangeIndexSlot, Qt::UniqueConnection);
		}
		m_widgetCams[0]->setGeometry(rt);
		m_widgetCams[0]->show();
	
		for (int j = 0; j < M_MAX_SEG; j++)
		{
			if (j >= nqs * nqs && m_widgetCams[j])
			{
				m_widgetCams[j]->hide();
				m_widgetCams[j]->SetInfo("");
				m_widgetCams[j]->CloseStream();
			}
		}
		break;
	case 2:
	default:
	{
		//4 分屏 w ratio = 557,18,557  h=346,16,346
		//9 w=257,21,257,21,257 h = 418,20,418,20,418
		float nww = rt.width() * 1.0f / (557 + 18 + 557) * 557;
		float nwh = rt.height() * 1.0f / (346 + 16 + 346) * 346;
		for (int i = 0; i < nqs; i++)
		{
			for (int j = 0; j < nqs; j++)
			{
				if (!m_widgetCams[i * nqs + j])
				{
					m_widgetCams[i * nqs + j] = new WidgetCam(this);
					m_widgetCams[i * nqs + j]->SetIndex(i * nqs + j);
					connect(m_widgetCams[i * nqs + j], &WidgetCam::IndexChangeSignal, this, &WidgetVideoSegment::ChangeIndexSlot, Qt::UniqueConnection);

				}
				
				m_widgetCams[i * nqs + j]->setGeometry((nww + 18.0) * j, (nwh + 16.0) * i, nww, nwh);
				m_widgetCams[i * nqs + j]->show();
			}
		}

		for (int j = 0; j < M_MAX_SEG; j++)
		{
			if (j >= nqs * nqs && m_widgetCams[j])
			{
				m_widgetCams[j]->hide();
				m_widgetCams[j]->SetInfo("");
				m_widgetCams[j]->CloseStream();
			}
		}
		break;
	}
	case 3:
	{
		//4 分屏 w ratio = 557,18,557  h=346,16,346
		//9 h=257,21,257,21,257 w = 418,20,418,20,418
		float nww = rt.width() * 1.0f / (418 + 20 + 418 + 20 + 418) * 418;
		float nwh = rt.height() * 1.0f / (257 + 20 + 257 + 20 + 257) * 257;
		for (int i = 0; i < nqs; i++)
		{
			for (int j = 0; j < nqs; j++)
			{
				if (!m_widgetCams[i * nqs + j])
				{
					m_widgetCams[i * nqs + j] = new WidgetCam(this);
					m_widgetCams[i * nqs + j]->SetIndex(i * nqs + j);
					connect(m_widgetCams[i * nqs + j], &WidgetCam::IndexChangeSignal, this, &WidgetVideoSegment::ChangeIndexSlot, Qt::UniqueConnection);

				}
				m_widgetCams[i * nqs + j]->setGeometry((nww + 20.0) * j, (nwh + 20.0) * i, nww, nwh);
				m_widgetCams[i * nqs + j]->show();
			}
		}

		for (int j = 0; j < M_MAX_SEG; j++)
		{
			if (j >= nqs * nqs && m_widgetCams[j])
			{
				m_widgetCams[j]->hide();
				m_widgetCams[j]->SetInfo("");
				m_widgetCams[j]->CloseStream();
			}
		}
		break;
	}
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	{
		float nww = rt.width() - nqs * 12.0 / nqs;
		float nwh = rt.height() - nqs * 10.0 / nqs;
		for (int i = 0; i < nqs; i++)
		{
			for (int j = 0; j < nqs; j++)
			{
				if (!m_widgetCams[i * nqs + j])
				{
					m_widgetCams[i * nqs + j] = new WidgetCam(this);
					m_widgetCams[i * nqs + j]->SetIndex(i * nqs + j);
					connect(m_widgetCams[i * nqs + j], &WidgetCam::IndexChangeSignal, this, &WidgetVideoSegment::ChangeIndexSlot, Qt::UniqueConnection);

				}
				m_widgetCams[i * nqs + j]->setGeometry((nww + 12.0) * j, (nwh + 10.0) * i, nww, nwh);
				m_widgetCams[i * nqs + j]->show();
			}
		}

		for (int j = 0; j < M_MAX_SEG; j++)
		{
			if (j >= nqs * nqs && m_widgetCams[j])
			{
				m_widgetCams[j]->hide();
			}
		}
		break;
	}
	}

	return true;
}

void WidgetVideoSegment::CloseAllStream()
{
	m_lsDeviceId->clear();

	for (int j = 0; j < M_MAX_SEG; j++)
	{
		if (m_widgetCams[j] && !m_widgetCams[j]->isHidden())
			m_widgetCams[j]->CloseStream();
	}
}

void WidgetVideoSegment::OpenStreamSlot(QString deviceId, int nPos, std::string url, QString szinfo, bool blAutoInc)
{
	if (nPos >= 0 && nPos < m_nSegment)
	{
		m_lsDeviceId[nPos] = deviceId;
		if (m_widgetCams[nPos])
		{
			m_widgetCams[nPos]->SetInfo(szinfo);
			m_widgetCams[nPos]->OpenStream(url);
		}

	}
	else
	{
		m_lsDeviceId[m_nCurrentPos] = deviceId;
		if (m_widgetCams[m_nCurrentPos])
		{
			m_widgetCams[m_nCurrentPos]->SetInfo(szinfo);
			m_widgetCams[m_nCurrentPos]->OpenStream(url);
		}
	}
	if (blAutoInc)
	{
		m_nCurrentPos++;
		if (m_nCurrentPos >= m_nSegment)
			m_nCurrentPos = 0;
	}
}

QStringList WidgetVideoSegment::GetCurrentDeviceIds()
{
	QStringList ls;

	for (int j = 0; j < M_MAX_SEG; j++)
		ls.append(m_lsDeviceId[j]);

	return ls;
}

void WidgetVideoSegment::ResizeUI(QRect rt)
{
	this->setGeometry(0, 0, rt.width(), rt.height());
	int nqs = sqrt(m_nSegment);

	switch (nqs)
	{
	case 1:
		if (!m_widgetCams[0])
		{
			m_widgetCams[0] = new WidgetCam(this);
			m_widgetCams[0]->SetIndex(0);
			connect(m_widgetCams[0], &WidgetCam::IndexChangeSignal, this, &WidgetVideoSegment::ChangeIndexSlot, Qt::UniqueConnection);
			connect(m_widgetCams[0], &WidgetCam::ShowDomeVideoSignal, this, &WidgetVideoSegment::ShowFullSlot, Qt::UniqueConnection);
		}
		m_widgetCams[0]->setGeometry(rt);
		m_widgetCams[0]->show();

		for (int j = 0; j < M_MAX_SEG; j++)
		{
			if (j >= nqs * nqs && m_widgetCams[j])
			{
				m_widgetCams[j]->hide();
				m_widgetCams[j]->SetInfo("");
				m_widgetCams[j]->CloseStream();
			}
		}
		break;
	case 2:
	default:
	{
		//4 分屏 w ratio = 557,18,557  h=346,16,346
		//9 w=257,21,257,21,257 h = 418,20,418,20,418
		float nww = rt.width() * 1.0f / (557 + 18 + 557) * 557;
		float nwh = rt.height() * 1.0f / (346 + 16 + 346) * 346;
		for (int i = 0; i < nqs; i++)
		{
			for (int j = 0; j < nqs; j++)
			{
				if (!m_widgetCams[i * nqs + j])
				{
					m_widgetCams[i * nqs + j] = new WidgetCam(this);
					m_widgetCams[i * nqs + j]->SetIndex(i * nqs + j);
					connect(m_widgetCams[i * nqs + j], &WidgetCam::IndexChangeSignal, this, &WidgetVideoSegment::ChangeIndexSlot, Qt::UniqueConnection);
					connect(m_widgetCams[i * nqs + j], &WidgetCam::ShowDomeVideoSignal, this, &WidgetVideoSegment::ShowFullSlot, Qt::UniqueConnection);
				}

				m_widgetCams[i * nqs + j]->setGeometry((nww + 18.0) * j, (nwh + 16.0) * i , nww, nwh);
				m_widgetCams[i * nqs + j]->show();
			}
		}
		for (int j = 0; j < M_MAX_SEG; j++)
		{
			if (j >= nqs * nqs && m_widgetCams[j])
			{
				m_widgetCams[j]->hide();
			}
		}
		break;
	}
	case 3:
	{
		//4 分屏 w ratio = 557,18,557  h=346,16,346
		//9 h=257,21,257,21,257 w = 418,20,418,20,418
		float nww = rt.width() * 1.0f / (418 + 20 + 418 + 20 + 418) * 418;
		float nwh = rt.height() * 1.0f / (257 + 20 + 257 + 20 + 257) * 257;
		for (int i = 0; i < nqs; i++)
		{
			for (int j = 0; j < nqs; j++)
			{
				if (!m_widgetCams[i * nqs + j])
				{
					m_widgetCams[i * nqs + j] = new WidgetCam(this);
					m_widgetCams[i * nqs + j]->SetIndex(i * nqs + j);
					connect(m_widgetCams[i * nqs + j], &WidgetCam::IndexChangeSignal, this, &WidgetVideoSegment::ChangeIndexSlot, Qt::UniqueConnection);
					connect(m_widgetCams[i * nqs + j], &WidgetCam::ShowDomeVideoSignal, this, &WidgetVideoSegment::ShowFullSlot, Qt::UniqueConnection);
				}
				m_widgetCams[i * nqs + j]->setGeometry((nww + 20.0) * j, (nwh + 20.0) * i, nww, nwh);
				m_widgetCams[i * nqs + j]->show();
			}
		}
		for (int j = 0; j < M_MAX_SEG; j++)
		{
			if (j >= nqs * nqs && m_widgetCams[j])
			{
				m_widgetCams[j]->hide();
				m_widgetCams[j]->SetInfo("");
				m_widgetCams[j]->CloseStream();
			}
		}
		break;
	}
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	{
		float nww = rt.width() - nqs * 12.0 / nqs;
		float nwh = rt.height() - nqs * 10.0 / nqs;
		for (int i = 0; i < nqs; i++)
		{
			for (int j = 0; j < nqs; j++)
			{
				if (!m_widgetCams[i * nqs + j])
				{
					m_widgetCams[i * nqs + j] = new WidgetCam(this);
					m_widgetCams[i * nqs + j]->SetIndex(i * nqs + j);
					connect(m_widgetCams[i * nqs + j], &WidgetCam::IndexChangeSignal, this, &WidgetVideoSegment::ChangeIndexSlot, Qt::UniqueConnection);
					connect(m_widgetCams[i * nqs + j], &WidgetCam::ShowDomeVideoSignal, this, &WidgetVideoSegment::ShowFullSlot, Qt::UniqueConnection);
				}
				m_widgetCams[i * nqs + j]->setGeometry((nww + 12.0) * j, (nwh + 10.0) * i, nww, nwh);
				m_widgetCams[i * nqs + j]->show();
			}
		}
		for (int j = 0; j < M_MAX_SEG; j++)
		{
			if (j >= nqs * nqs && m_widgetCams[j])
			{
				m_widgetCams[j]->hide();
				m_widgetCams[j]->SetInfo("");
				m_widgetCams[j]->CloseStream();
			}
		}
		break;
	}
	}
}
void WidgetVideoSegment::ShowFullSlot(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_nSegment)
	{
		m_nCurrentPos = nIndex;

		emit ShowFullIndexSignal(m_lsDeviceId[m_nCurrentPos]);
	}
}

void WidgetVideoSegment::ChangeIndexSlot(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_nSegment)
	{
		m_nCurrentPos = nIndex;
		for (int ii = 0; ii < M_MAX_SEG; ii++)
		{
			if (ii < m_nSegment && m_widgetCams[ii])
				m_widgetCams[ii]->SetDefault(ii == nIndex);
		}
		emit IndexChangeSignal(m_nCurrentPos, m_lsDeviceId[m_nCurrentPos]);
	}
}
