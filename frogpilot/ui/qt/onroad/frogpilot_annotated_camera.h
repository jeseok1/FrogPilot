#pragma once

#include "selfdrive/ui/qt/onroad/buttons.h"
#include "selfdrive/ui/qt/widgets/cameraview.h"

const int widget_size = img_size + (UI_BORDER_SIZE / 2);

class FrogPilotAnnotatedCameraWidget : public QWidget {
  Q_OBJECT

public:
  explicit FrogPilotAnnotatedCameraWidget(QWidget *parent = 0);

  void paintAdjacentPaths(QPainter &p, SubMaster &fpsm, const FrogPilotUIScene &frogpilot_scene, const QJsonObject &frogpilot_toggles);
  void paintBlindSpotPath(QPainter &p, SubMaster &fpsm, const FrogPilotUIScene &frogpilot_scene);
  void paintFrogPilotWidgets(QPainter &p, UIState &s, FrogPilotUIState &fs, SubMaster &sm, SubMaster &fpsm, QJsonObject &frogpilot_toggles);
  void paintLeadMetrics(QPainter &p, bool adjacent, QPointF *chevron, SubMaster &fpsm, const cereal::RadarState::LeadData::Reader &lead_data);
  void paintPathEdges(QPainter &p, const UIScene &scene, const FrogPilotUIScene &frogpilot_scene, SubMaster &sm);
  void paintRainbowPath(QPainter &p, QLinearGradient &bg, float lin_grad_point, SubMaster &sm);
  void updateState(const FrogPilotUIState &fs, const QJsonObject &frogpilot_toggles);

  bool hideBottomIcons;
  bool isCruiseSet;
  bool mutcdSpeedLimit;
  bool rightHandDM;
  bool viennaSpeedLimit;

  int alertHeight;
  int frogHopCount;
  int signMargin;
  int standstillDuration;

  float distanceConversion;
  float setSpeed;
  float speed;
  float speedConversion;
  float speedConversionMetrics;

  QColor blueColor(int alpha = 255) { return QColor(0, 0, 255, alpha); }

  QPoint dmIconPosition;
  QPoint experimentalButtonPosition;

  QPolygonF track_vertices;

  QRect leadTextRect;
  QRect newSpeedLimitRect;
  QRect setSpeedRect;
  QRect speedLimitRect;

  QSize defaultSize;

  QString accelerationUnit;
  QString leadDistanceUnit;
  QString leadSpeedUnit;
  QString signalStyle;
  QString speedLimitOffsetStr;
  QString speedUnit;

protected:
  void showEvent(QShowEvent *event) override;

private:
  void paintCEMStatus(QPainter &p, FrogPilotUIScene &frogpilot_scene, SubMaster &sm);
  void paintCompass(QPainter &p, QJsonObject &frogpilot_toggles);
  void paintCurveSpeedControl(QPainter &p, SubMaster &fpsm);
  void paintCurveSpeedControlTraining(QPainter &p, SubMaster &fpsm);
  void paintLateralPaused(QPainter &p, FrogPilotUIScene &frogpilot_scene);
  void paintLongitudinalPaused(QPainter &p, FrogPilotUIScene &frogpilot_scene);
  void paintPedalIcons(QPainter &p, SubMaster &fpsm, FrogPilotUIScene &frogpilot_scene, QJsonObject &frogpilot_toggles);
  void paintPendingSpeedLimit(QPainter &p, SubMaster &fpsm);
  void paintRadarTracks(QPainter &p, UIState &s, FrogPilotUIScene &frogpilot_scene, SubMaster &sm, SubMaster &fpsm);
  void paintRoadName(QPainter &p);
  void paintSpeedLimitSources(QPainter &p, SubMaster &fpsm);
  void paintStandstillTimer(QPainter &p);
  void paintStoppingPoint(QPainter &p, UIScene &scene, FrogPilotUIScene &frogpilot_scene, QJsonObject &frogpilot_toggles);
  void paintTurnSignals(QPainter &p, SubMaster &fpsm);
  void updateSignals();

  int animationFrameIndex;
  int signalAnimationLength;
  int signalHeight;
  int signalMovement;
  int signalWidth;
  int totalFrames;

  Params params;
  Params params_memory{"", false, true};

  QColor blackColor(int alpha = 255) { return QColor(0, 0, 0, alpha); }
  QColor redColor(int alpha = 255) { return QColor(201, 34, 49, alpha); }
  QColor whiteColor(int alpha = 255) { return QColor(255, 255, 255, alpha); }

  QElapsedTimer glowTimer;
  QElapsedTimer pendingLimitTimer;
  QElapsedTimer standstillTimer;

  QPixmap brakePedalImg;
  QPixmap curveSpeedIcon;
  QPixmap dashboardIcon;
  QPixmap gasPedalImg;
  QPixmap mapboxIcon;
  QPixmap mapDataIcon;
  QPixmap nextMapsIcon;
  QPixmap pausedIcon;
  QPixmap speedIcon;
  QPixmap stopSignImg;
  QPixmap turnIcon;

  QPoint cemStatusPosition;
  QPoint compassPosition;
  QPoint lateralPausedPosition;

  QSharedPointer<QMovie> cemCurveIcon;
  QSharedPointer<QMovie> cemLeadIcon;
  QSharedPointer<QMovie> cemSpeedIcon;
  QSharedPointer<QMovie> cemStopIcon;
  QSharedPointer<QMovie> cemTurnIcon;
  QSharedPointer<QMovie> chillModeIcon;
  QSharedPointer<QMovie> experimentalModeIcon;

  QTimer *animationTimer;

  QVector<QPixmap> blindspotImages;
  QVector<QPixmap> signalImages;
};
