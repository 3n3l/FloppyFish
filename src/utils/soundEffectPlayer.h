#ifndef SOUNDEFFECTPLAYER_H
#define SOUNDEFFECTPLAYER_H

#include <QObject>
#include <QSoundEffect>

class SoundEffectPlayer : public QObject {
    Q_OBJECT

   public:
    explicit SoundEffectPlayer(QObject* parent = nullptr);

   signals:
    void playJumpSFX();

   public slots:
    void handlePlayJumpSFX();
};

#endif  // SOUNDEFFECTPLAYER_H
