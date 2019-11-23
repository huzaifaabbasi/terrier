#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "parser/parameter.h"
#include "parser/update_statement.h"
#include "planner/plannodes/abstract_plan_node.h"
#include "planner/plannodes/output_schema.h"

namespace terrier::planner {

/**
 * Plan node for update
 */
class UpdatePlanNode : public AbstractPlanNode {
 public:
  /**
   * Builder for an delete plan node
   */
  class Builder : public AbstractPlanNode::Builder<Builder> {
   public:
    Builder() = default;

    /**
     * Don't allow builder to be copied or moved
     */
    DISALLOW_COPY_AND_MOVE(Builder)

    /**
     * @param database_oid OID of the database
     * @return builder object
     */
    Builder &SetDatabaseOid(catalog::db_oid_t database_oid) {
      database_oid_ = database_oid;
      return *this;
    }

    /**
     * @param namespace_oid OID of the namespace
     * @return builder object
     */
    Builder &SetNamespaceOid(catalog::namespace_oid_t namespace_oid) {
      namespace_oid_ = namespace_oid;
      return *this;
    }

    /**
     * @param table_oid the OID of the target SQL table
     * @return builder object
     */
    Builder &SetTableOid(catalog::table_oid_t table_oid) {
      table_oid_ = table_oid;
      return *this;
    }

    /**
     * @param update_primary_key whether to update primary key
     * @return builder object
     */
    Builder &SetUpdatePrimaryKey(bool update_primary_key) {
      update_primary_key_ = update_primary_key;
      return *this;
    }

    /**
     * @param values values to update
     * @return builder object
     */
    Builder &SetValues(std::vector<type::TransientValue> &&values) {
      values_ = std::move(values);
      return *this;
    }

    /**
     * @param parameter_info vector of col_oid's where value at value_idx should be updated
     * @return builder object
     */
    Builder &SetParameterInfo(std::vector<catalog::col_oid_t> parameter_info) {
      parameter_info_ = std::move(parameter_info);
      return *this;
    }

    /**
     * @param col_oid oid of column where value at value_idx should be updated
     * @return builder object
     * @warning The caller must push column index in order. The ith call to AddParameterInfo means for a value tuple
     * values_, values_[i] will be updated in the column indicated by the input col_oid.
     */
    Builder &AddParameterInfo(catalog::col_oid_t col_oid) {
      parameter_info_.emplace_back(col_oid);
      return *this;
    }

    /**
     * @param update_condition expression of update condition
     * @return builder object
     */
    Builder &SetUpdateCondition(std::shared_ptr<parser::AbstractExpression> &&update_condition) {
      update_condition_ = std::move(update_condition);
      return *this;
    }

    /**
     * @param update_stmt the SQL UPDATE statement
     * @return builder object
     */
    Builder &SetFromUpdateStatement(parser::UpdateStatement *update_stmt) {
      update_condition_ = update_stmt->GetUpdateCondition();
      return *this;
    }

    /**
     * @param index_oids vector of index oids to insert into
     * @return builder object
     */
    Builder &SetIndexOids(std::vector<catalog::index_oid_t> &&index_oids) {
      index_oids_ = std::move(index_oids);
      return *this;
    }

    /**
     * Build the delete plan node
     * @return plan node
     */
    std::shared_ptr<UpdatePlanNode> Build() {
      return std::shared_ptr<UpdatePlanNode>(
          new UpdatePlanNode(std::move(children_), std::move(output_schema_), database_oid_, namespace_oid_, table_oid_,
                             update_primary_key_, std::move(values_), std::move(parameter_info_),
                             std::move(update_condition_), std::move(index_oids_)));
    }

   protected:
    /**
     * OID of the database
     */
    catalog::db_oid_t database_oid_;

    /**
     * OID of namespace
     */
    catalog::namespace_oid_t namespace_oid_;

    /**
     * OID of the table to update
     */
    catalog::table_oid_t table_oid_;

    /**
     * Whether to update primary key
     */
    bool update_primary_key_;

    /**
     * vector of values to update. Parameter info provides column mapping of values
     */
    std::vector<type::TransientValue> values_;

    /**
     * parameter information. Provides which column a value should be updated for.
     * For example, for the column parameter_info_[i], the value should be updated to values_[i]
     */
    std::vector<catalog::col_oid_t> parameter_info_;

    /**
     * Expression of update condition
     */
    std::shared_ptr<parser::AbstractExpression> update_condition_;

    /**
     * vector of indexes used by this node
     */
    std::vector<catalog::index_oid_t> index_oids_;
  };

 private:
  /**
   * @param children child plan nodes
   * @param output_schema Schema representing the structure of the output of this plan node
   * @param database_oid OID of the database
   * @param namespace_oid OID of the namespace
   * @param table_oid OID of the target SQL table
   * @param update_primary_key whether to update primary key
   * @param values values to update
   * @param parameter_info parameters information
   * @param update_condition expression of update condition
   * @param index_oids vector of indexes used by this node
   * TODO(thepulkitagarwal): There shouldn't be any children here (?)
   */
  UpdatePlanNode(std::vector<std::shared_ptr<AbstractPlanNode>> &&children, std::shared_ptr<OutputSchema> output_schema,
                 catalog::db_oid_t database_oid, catalog::namespace_oid_t namespace_oid, catalog::table_oid_t table_oid,
                 bool update_primary_key, std::vector<type::TransientValue> &&values,
                 std::vector<catalog::col_oid_t> &&parameter_info,
                 std::shared_ptr<parser::AbstractExpression> &&update_condition,
                 std::vector<catalog::index_oid_t> &&index_oids)
      : AbstractPlanNode(std::move(children), std::move(output_schema)),
        database_oid_(database_oid),
        namespace_oid_(namespace_oid),
        table_oid_(table_oid),
        update_primary_key_(update_primary_key),
        values_(std::move(values)),
        parameter_info_(std::move(parameter_info)),
        update_condition_(std::move(update_condition)),
        index_oids_(std::move(index_oids)) {}

 public:
  /**
   * Default constructor for deserialization
   */
  UpdatePlanNode() = default;

  DISALLOW_COPY_AND_MOVE(UpdatePlanNode)

  /**
   * @return OID of the database
   */
  catalog::db_oid_t GetDatabaseOid() const { return database_oid_; }

  /**
   * @return OID of the namespace
   */
  catalog::namespace_oid_t GetNamespaceOid() const { return namespace_oid_; }

  /**
   * @return the OID of the target table to operate on
   */
  catalog::table_oid_t GetTableOid() const { return table_oid_; }

  /**
   * @return whether to update primary key
   */
  bool GetUpdatePrimaryKey() const { return update_primary_key_; }

  /**
   * @return values to be updated
   */
  const std::vector<type::TransientValue> &GetValues() const { return values_; }

  /**
   * @return the information of update parameters
   */
  const std::vector<catalog::col_oid_t> &GetParameterInfo() const { return parameter_info_; }

  /**
   * @param value_idx index of value being updated
   * @return OID of column where value should be updated
   */
  catalog::col_oid_t GetColumnOidForValue(const uint32_t value_idx) const { return parameter_info_.at(value_idx); }

  /**
   * @return the expression of update condition
   */
  std::shared_ptr<parser::AbstractExpression> GetUpdateCondition() const { return update_condition_; }

  /**
   * @return the type of this plan node
   */
  PlanNodeType GetPlanNodeType() const override { return PlanNodeType::UPDATE; }

  /**
   * @return the hashed value of this plan node
   */
  common::hash_t Hash() const override;

  bool operator==(const AbstractPlanNode &rhs) const override;

  nlohmann::json ToJson() const override;
  void FromJson(const nlohmann::json &j) override;

 private:
  /**
   * OID of the database
   */
  catalog::db_oid_t database_oid_;

  /**
   * OID of namespace
   */
  catalog::namespace_oid_t namespace_oid_;

  /**
   * OID of the table to update
   */
  catalog::table_oid_t table_oid_;

  /**
   * Whether to update primary key
   */
  bool update_primary_key_;

  /**
   * vector of values to update. Parameter info provides column mapping of values
   */
  std::vector<type::TransientValue> values_;

  /**
   * parameter information. Provides which column a value should be updated for.
   * For example, for the column parameter_info_[i], the value should be updated to values_[i]
   */
  std::vector<catalog::col_oid_t> parameter_info_;

  /**
   * Expression of update condition
   */
  std::shared_ptr<parser::AbstractExpression> update_condition_;

  /**
   * vector of indexes used by this node
   */
  std::vector<catalog::index_oid_t> index_oids_;
};

DEFINE_JSON_DECLARATIONS(UpdatePlanNode);

}  // namespace terrier::planner
