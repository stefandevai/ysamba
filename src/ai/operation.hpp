enum class OperationType
{
  None,
  Harvest,
  Store,
};

struct Operation
{
  OperationType type;
  double score = 0.0;
};
