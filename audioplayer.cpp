#include "audioplayer.h"
#include <QDir>
#include <QMediaPlayer>
#include <QMediaPlaylist>

static const QString s_curDir = QDir::currentPath() + "/";

AudioPlayer::AudioPlayer(QObject *parent)
	: QObject(parent)
	, m_backgroundMusic(NULL)
{
	// 创建一直播放的背景音乐
    QUrl backgroundMusicUrl = QUrl::fromLocalFile(s_curDir + "music/bgm.mp3");
	if (QFile::exists(backgroundMusicUrl.toLocalFile()))
	{
        m_backgroundMusic = new QMediaPlayer(this);
        QMediaPlaylist *backgroundMusicList = new QMediaPlaylist();

        QMediaContent media(backgroundMusicUrl);
        backgroundMusicList->addMedia(media);
        backgroundMusicList->setCurrentIndex(0);
		// 设置背景音乐循环播放
        backgroundMusicList->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        m_backgroundMusic->setPlaylist(backgroundMusicList);

       // m_backgroundMusic = new QMediaPlayer;
       // m_backgroundMusic->setMedia(QUrl::fromLocalFile(s_curDir + "music/bgm.mp3"));
        //m_backgroundMusic->setVolume(80);
        //m_backgroundMusic->play();
	}
}

void AudioPlayer::startBGM()
{
	if (m_backgroundMusic)
		m_backgroundMusic->play();
}

void AudioPlayer::playSound(SoundType soundType)
{
	static const QUrl mediasUrls[] =
	{
		QUrl::fromLocalFile(s_curDir + "music/tower_place.wav"),
		QUrl::fromLocalFile(s_curDir + "music/life_lose.wav"),
		QUrl::fromLocalFile(s_curDir + "music/laser_shoot.wav"),
		QUrl::fromLocalFile(s_curDir + "music/enemy_destroy.wav")
	};
	static QMediaPlayer player;

	if (QFile::exists(mediasUrls[soundType].toLocalFile()))
	{
		player.setMedia(mediasUrls[soundType]);
       // player.setVolume(100);
		player.play();
	}
}
