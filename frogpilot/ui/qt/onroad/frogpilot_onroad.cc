#include "frogpilot/ui/qt/onroad/frogpilot_onroad.h"

FrogPilotOnroadWindow::FrogPilotOnroadWindow(QWidget *parent) : QWidget(parent) {
  signalTimer = new QTimer(this);
  QObject::connect(signalTimer, &QTimer::timeout, [this] {
    flickerActive = !flickerActive;
  });
}

void FrogPilotOnroadWindow::resizeEvent(QResizeEvent *event) {
  rect = QWidget::rect();
  marginRegion = QRegion(rect) - QRegion(rect.marginsRemoved(QMargins(UI_BORDER_SIZE, UI_BORDER_SIZE, UI_BORDER_SIZE, UI_BORDER_SIZE)));
}

void FrogPilotOnroadWindow::updateState(const UIState &s, const FrogPilotUIState &fs) {
  const SubMaster &sm = *(s.sm);
  const SubMaster &fpsm = *(fs.sm);

  const cereal::CarState::Reader &carState = sm["carState"].getCarState();
  const cereal::CarControl::Reader &carControl = fpsm["carControl"].getCarControl();

  const cereal::CarOutput::Reader &carOutput = fpsm["carOutput"].getCarOutput();

  blindSpotLeft = carState.getLeftBlindspot();
  blindSpotRight = carState.getRightBlindspot();
  latActive = carControl.getLatActive();
  steeringAngleDeg = carState.getSteeringAngleDeg();
  steeringTorque = carState.getSteeringTorque();
  torque = -carControl.getActuators().getTorque();
  torqueOutputCan = carOutput.getActuatorsOutput().getTorqueOutputCan();
  turnSignalLeft = carState.getLeftBlinker();
  turnSignalRight = carState.getRightBlinker();

  showBlindspot = (blindSpotLeft || blindSpotRight) && frogpilot_toggles.value("blind_spot_metrics").toBool();
  showFPS = frogpilot_toggles.value("show_fps").toBool();
  showSignal = (turnSignalLeft || turnSignalRight) && frogpilot_toggles.value("signal_metrics").toBool();
  showSteering = frogpilot_toggles.value("steering_metrics").toBool();

  if (showSteering) {
    float absTorque = std::abs(torque);
    smoothedSteer = 0.25f * absTorque + 0.75f * smoothedSteer;
    if (std::abs(smoothedSteer - absTorque) < 0.01f) {
      smoothedSteer = absTorque;
    }
  }

  if (showBlindspot || showSignal) {
    std::function<QColor(bool, bool)> getBorderColor = [&](bool blindSpot, bool turnSignal) {
      if (turnSignal && showSignal) {
        if (blindSpot) {
          return flickerActive ? bg_colors[STATUS_TRAFFIC_MODE_ENABLED] : bg_colors[STATUS_CEM_DISABLED];
        } else {
          return flickerActive ? bg_colors[STATUS_CEM_DISABLED] : bg;
        }
      } else if (blindSpot && showBlindspot) {
        return bg_colors[STATUS_TRAFFIC_MODE_ENABLED];
      } else {
        return bg;
      }
    };

    leftBorderColor = getBorderColor(blindSpotLeft, turnSignalLeft);
    rightBorderColor = getBorderColor(blindSpotRight, turnSignalRight);

    int interval = showBlindspot ? 250 : 500;
    if (!signalTimer->isActive() || signalTimer->interval() != interval) {
      signalTimer->start(interval);
    }
  } else if (signalTimer->isActive()) {
    signalTimer->stop();
  }

  if (showFPS) {
    static float avgFPS = 0.0f;
    static float maxFPS = 0.0f;
    static float minFPS = 99.9f;

    if (avgFPS == 0.0f) {
      avgFPS = fps;
    }

    static float alpha = 1.0f / (UI_FREQ * 60.0f);
    avgFPS = alpha * fps + (1.0f - alpha) * avgFPS;

    minFPS = std::min(minFPS, fps);
    maxFPS = std::max(maxFPS, fps);

    fpsDisplayString = QString("FPS: %1 | Min: %2 | Max: %3 | Avg: %4")
                          .arg(qRound(fps))
                          .arg(qRound(minFPS))
                          .arg(qRound(maxFPS))
                          .arg(qRound(avgFPS));
  }

  update();
}

void FrogPilotOnroadWindow::paintEvent(QPaintEvent *event) {
  QPainter p(this);

  p.setClipRegion(marginRegion);
  p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  if (showSteering) {
    paintSteeringTorqueBorder(p);
    paintSteeringDebugText(p);
  }

  if (showBlindspot || showSignal) {
    paintTurnSignalBorder(p);
  }

  if (showFPS) {
    paintFPS(p);
  }
}

void FrogPilotOnroadWindow::paintFPS(QPainter &p) {
  p.save();

  p.setFont(InterFont(28, QFont::DemiBold));
  p.setPen(Qt::white);

  int xPos = (rect.width() - p.fontMetrics().horizontalAdvance(fpsDisplayString)) / 2;
  int yPos = rect.bottom() - 5;

  p.drawText(xPos, yPos, fpsDisplayString);

  p.restore();
}

void FrogPilotOnroadWindow::paintSteeringTorqueBorder(QPainter &p) {
  p.save();

  QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
  gradient.setColorAt(0.0, bg_colors[STATUS_TRAFFIC_MODE_ENABLED]);
  gradient.setColorAt(0.25, bg_colors[STATUS_EXPERIMENTAL_MODE_ENABLED]);
  gradient.setColorAt(0.5, bg_colors[STATUS_CEM_DISABLED]);
  gradient.setColorAt(0.75, bg_colors[STATUS_ENGAGED]);

  int visibleHeight = rect.height() * smoothedSteer;
  int xPos = (torque < 0) ? rect.x() : (rect.x() + rect.width() - UI_BORDER_SIZE);
  int yPos = rect.y() + rect.height() - visibleHeight;

  p.fillRect(QRect(xPos, yPos, UI_BORDER_SIZE, visibleHeight), gradient);

  p.restore();
}

void FrogPilotOnroadWindow::paintSteeringDebugText(QPainter &p) {
  p.save();
  p.setClipRect(rect);

  int fontSize = 28;
  int lineHeight = 34;
  int x = rect.x() + UI_BORDER_SIZE + 16;
  int y = rect.bottom() - UI_BORDER_SIZE - lineHeight * 3 - 8;

  // background
  int bgW = 230;
  int bgH = lineHeight * 3 + 12;
  p.fillRect(QRect(x - 8, y - 4, bgW, bgH), QColor(0, 0, 0, 120));

  QColor textColor = latActive ? QColor(128, 216, 166, 220) : QColor(180, 180, 180, 180);
  p.setFont(InterFont(fontSize, QFont::DemiBold));
  p.setPen(textColor);

  // Line 1: Applied torque (CAN value)
  p.drawText(x, y + lineHeight - 4, QString("STEER %1").arg(static_cast<int>(torqueOutputCan), 4, 10, QChar(' ')));
  // Line 2: Normalized torque
  p.drawText(x, y + lineHeight * 2 - 4, QString("TORQ  %1").arg(torque, 0, 'f', 2));
  // Line 3: Driver torque + steering angle
  p.drawText(x, y + lineHeight * 3 - 4, QString("DRV %1  %2%3")
    .arg(static_cast<int>(steeringTorque), 3, 10, QChar(' '))
    .arg(steeringAngleDeg, 0, 'f', 1)
    .arg(QChar(0x00B0)));

  p.restore();
}

void FrogPilotOnroadWindow::paintTurnSignalBorder(QPainter &p) {
  p.save();

  p.fillRect(rect.x(), rect.y(), rect.width() / 2, rect.height(), leftBorderColor);
  p.fillRect(rect.x() + rect.width() / 2, rect.y(), rect.width() / 2, rect.height(), rightBorderColor);

  p.restore();
}
