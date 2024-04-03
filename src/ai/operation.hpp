enum class OperationType
{
  None,
  Harvest,
  Store,
  Eat,
};

struct Operation
{
  OperationType type;
  double score = 0.0;
};
