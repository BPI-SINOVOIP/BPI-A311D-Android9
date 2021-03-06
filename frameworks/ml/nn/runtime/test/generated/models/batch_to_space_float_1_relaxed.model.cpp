// Generated file (from: batch_to_space_float_1_relaxed.mod.py). Do not edit
void CreateModel(Model *model) {
  OperandType type2(Type::TENSOR_FLOAT32, {1, 4, 4, 1});
  OperandType type0(Type::TENSOR_FLOAT32, {4, 2, 2, 1});
  OperandType type1(Type::TENSOR_INT32, {2});
  // Phase 1, operands
  auto input = model->addOperand(&type0);
  auto block_size = model->addOperand(&type1);
  auto output = model->addOperand(&type2);
  // Phase 2, operations
  static int32_t block_size_init[] = {2, 2};
  model->setOperandValue(block_size, block_size_init, sizeof(int32_t) * 2);
  model->addOperation(ANEURALNETWORKS_BATCH_TO_SPACE_ND, {input, block_size}, {output});
  // Phase 3, inputs and outputs
  model->identifyInputsAndOutputs(
    {input},
    {output});
  // Phase 4: set relaxed execution
  model->relaxComputationFloat32toFloat16(true);
  assert(model->isValid());
}

bool is_ignored(int i) {
  static std::set<int> ignore = {};
  return ignore.find(i) != ignore.end();
}
