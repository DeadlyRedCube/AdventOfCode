class Timer
{
public:
  Timer() noexcept
    : lastCheckpointTime(std::chrono::high_resolution_clock::now())
    { }

  f32 SecondsSinceLastCheckpoint() const noexcept
    { return std::chrono::duration<f32>(std::chrono::high_resolution_clock::now() - lastCheckpointTime).count(); }

  u64 MillisecondsSinceLastCheckpoint() const noexcept
    { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastCheckpointTime).count(); }

  f32 CheckpointAsSeconds() noexcept
  {
    auto now = std::chrono::high_resolution_clock::now();
    f32 result = std::chrono::duration<f32>(now - lastCheckpointTime).count();
    lastCheckpointTime = now;
    return result;
  }

  u64 CheckpointAsMilliseconds() noexcept
  {
    auto now = std::chrono::high_resolution_clock::now();
    u64 result = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastCheckpointTime).count();
    lastCheckpointTime = now;
    return result;
  }

protected:
  std::chrono::time_point<std::chrono::high_resolution_clock> lastCheckpointTime;
};
