<p align="center">
  <picture>
    <img alt="TypeTween Hero Image" src="https://github.com/user-attachments/assets/e8c3ef6d-0f45-4f4f-b86e-c528ec12da6d" />
  </picture>
</p>
<h1 align="center">
  <span>TypeTween</span>
</h1>
<p align="center">
  <span align="center"><b>Tweening library for Unreal Engine with a fluent C++ API and Blueprint nodes</b></span>
</p>

### C++

```cpp
TypeTween::Tween<float>(this)
  .From(0.0f)
  .To(10.0f)
  .Duration(2.5f)
  .Ease(ETweenEase::Linear);
```

### Blueprints

<!-- <img alt="tween float blueprint" src="https://github.com/user-attachments/assets/2b2e3f5c-3d9a-4539-bfd6-979dd3874d5a" /> -->
<img alt="tween float blueprint" src="https://github.com/user-attachments/assets/d3274d1c-2bee-4832-8922-b27997929a7f" />



## API Reference

### Creating a Tween

```cpp
// Tween an existing value (you manage the lifetime of the variable)
TypeTween::Tween(MyValue, this)

// Tween an internally-owned value (fire and forget)
TypeTween::Tween<float>(this)

// Tween with an initial value (type deduced)
TypeTween::Tween(FVector::ZeroVector, this)

// Tween an actor's transform
TypeTween::Tween(MyActor)

// Timing-only tween (no value)
TypeTween::Tween(this)
```

### Values

```cpp
.From(startValue)     // start value (defaults to current value if omitted)
.To(endValue)         // end value
.By(delta)            // relative end: To = From + By
```

### Easing ([Easings.net](https://easings.net/))

```cpp
.Ease(ETweenEase::OutBounce)
```

| Family  | Variants                                |
| ------- | --------------------------------------- |
| Linear  | `Linear`                                |
| Sine    | `InSine` `OutSine` `InOutSine`          |
| Quad    | `InQuad` `OutQuad` `InOutQuad`          |
| Cubic   | `InCubic` `OutCubic` `InOutCubic`       |
| Quart   | `InQuart` `OutQuart` `InOutQuart`       |
| Quint   | `InQuint` `OutQuint` `InOutQuint`       |
| Expo    | `InExpo` `OutExpo` `InOutExpo`          |
| Circ    | `InCirc` `OutCirc` `InOutCirc`          |
| Back    | `InBack` `OutBack` `InOutBack`          |
| Elastic | `InElastic` `OutElastic` `InOutElastic` |
| Bounce  | `InBounce` `OutBounce` `InOutBounce`    |


### Looping

```cpp
.Repeat(3)         // play 4 times total (default: 0 = once)
.Repeat(-1)        // Repeat infinitly until stopped
.PingPong(false)   //(Default) 0 -> 1, 0 -> 1...
.PingPong(true)    // 0 -> 1 -> 0...
```

### Timing

```cpp
.Duration(2.0f)       // animation length in seconds (required)
.StartDelay(0.5f)     // wait before starting
.ReverseDelay(0.3f)   // ping-pong: pause between forward and reverse phases
.RepeatDelay(0.2f)    // pause between repeat cycles
.EndDelay(0.1f)       // pause after the final cycle before OnComplete fires
```

### Callbacks

```cpp
.OnPreStart([]() { })                          // very first frame, before StartDelay
.OnStart([]() { })                             // after StartDelay elapses
.OnCycleBegin([]() { })                        // start of each cycle
.OnForwardEnd([]() { })                        // forward phase complete (alpha == 1)
.OnReverseBegin([]() { })                      // ping-pong: reverse phase begins
.OnCycleEnd([]() { })                          // end of cycle
.OnRepeat([]() { })                            // each repeat except the last
.OnComplete([]() { })                          // all cycles finished
.OnTick([]() { })                              // every frame, including delays
.OnUpdate([](float Alpha, const T& Value) { }) // typed, during interpolation
```

### Advanced Settings

Click the ▼ arrow for more settings and callbacks:

<picture>
  <img alt="Tween Float Blueprint Advanced" src="https://github.com/user-attachments/assets/cefd8f07-86d0-4e91-ac8b-53e334015a0b" />
</picture>

### Handles

Handles keep a tween alive and let you control it after creation:

```cpp
TypeTween::FTweenHandle Handle = TypeTween::Tween<float>(this)
    .To(1.f)
    .Duration(2.f);

Handle->Pause();
Handle->Resume();
Handle->Restart();
Handle->Finish();    // jump to end state immediately
Handle->IsDone();
Handle.Reset();      // release handle (tween is destroyed if no other refs remain)
```


## Default Types

Types like ``int``, ``double``, ``float``, ``FVector``, ``FRotator`` all supported by default:

```cpp
TypeTween::Tween<int>(this).From(0).To(100);
TypeTween::Tween<double>(this).From(0.0).To(1.0);
TypeTween::Tween<float>(this).From(0.0f).To(1.0f);
TypeTween::Tween<FVector>(this).From(FVector::ZeroVector).To(FVector(1.f, 2.f, 3.f));
TypeTween::Tween<FRotator>(this).From(FRotator::ZeroRotator).To(FRotator(45.f, 90.f, 180.f));
```

<picture>
  <img width="50%" alt="Blueprint Context window" src="https://github.com/user-attachments/assets/dc3c5f13-07cf-4d60-9d6b-cae6c0f7930f" />
</picture>

## Specialized Types

Some Types have custom functionality or helper functions:

### FTransform

``FTransform`` has custom ``From()``/``To()`` functions for each component:

```cpp
// Default:
TypeTween::Tween<FTransform>(this).From(StartTransform).To(EndTransform);

// Helper Functions:
TypeTween::Tween<FTransform>(this)
  /* Move */
  .MoveFrom(FVector(0.f, 0.f, 0.f))
  .MoveTo(FVector(100.f, 0.f, 0.f))
  /* Rotate */
  .RotateFrom(FRotator::ZeroRotator)
  .RotateTo(FRotator(0.f, 90.f, 0.f))
  /* Scale */
  .ScaleFrom(FVector(1.f, 1.f, 1.f))
  .ScaleTo(FVector(2.f, 2.f, 2.f));
```

### FLinearColor

Colors can be interpolated/lerped in different ways.

```
TypeTween::Tween<FLinearColor>(this)
  .From(FLinearColor::Yellow)
  .To(FLinearColor::Blue)
  /* Specify color interpolation mode: */
  .LerpMode(EColorLerpMode::sRGB)
  .LerpMode(EColorLerpMode::Linear)
  .LerpMode(EColorLerpMode::HSV)
  .LerpMode(EColorLerpMode::Oklab)
```

<picture>
  <source media="(prefers-color-scheme: light)" srcset="https://github.com/user-attachments/assets/ab991e1f-e8e7-471d-80a3-c0ceb87af5b9" />
  <img alt="TypeTween-gradients" src="https://github.com/user-attachments/assets/a4e0dbc6-a86b-4b1d-9194-847bfb6271cf" />
</picture>
