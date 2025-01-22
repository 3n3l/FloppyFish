#include "soundeffectplayer.h"

SoundEffectPlayer::SoundEffectPlayer(QObject *parent) : QObject(parent), _jumpSFX(new QSoundEffect(this)) {
    _jumpSFX->setSource(QUrl::fromLocalFile("res/FloppyJumpSFX.wav"));
    _jumpSFX->setVolume(100);
}

void SoundEffectPlayer::handlePlayJumpSFX() {
    if (!_jumpSFX->isPlaying()) {
        _jumpSFX->play();
    }
}
